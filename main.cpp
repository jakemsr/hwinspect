#include <algorithm>
#include <future>
#include <iostream>
#include <iomanip>
#include <ranges>
#include <sstream>
#include "usb_parser.hpp"
#include "pci_parser.hpp"
#include "db_lookup.hpp"


void
print_pci_devices(const std::string& pcidump) {

	auto devices = parse_pcidump_output(pcidump);
	if (devices.empty()) {
		std::cout << "No PCI devices found\n";
		return;
	}

	auto longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			return a.address.length() < b.address.length();
		});
	size_t max_address_len = longest->address.length();


	for (auto device: devices) {
		std::cout << device.controller << "  "
			<< std::left << std::setw(max_address_len) << device.address << "  "
			<< device.vendor_id << ":" << device.product_id << "  "
			<< device.pci_device_name << " "
			<< device.driver << '\n';
	}
}

int
main(int argc, char* argv[]) {
	std::string pcidump = "";
	
	for (int i = 1; i < argc; i++) {
		std::string flag = argv[i];
		if (flag[0] != '-' || flag.length() != 2) {
			std::cerr << "malformed option flag\n";
			exit(1);
		}
		switch (flag[1]) {
			case 'p':
				if (argv[++i]) {
					pcidump = argv[i];
				} else {
					std::cerr << argv[0] << " -p (pcidump output file)\n";
					exit(1);
				}
				break;
			default:
				std::cerr << argv[0] << " only accepts the -p option\n";
				exit(1);
				break;
		}
	}

	auto output = run_usbdevs();
	if (!output) {
		std::cerr << "Failed to read usbdevs\n";
		return 1;
	}

	std::istringstream input(*output);

	auto usb_devices = parse_usbdevs_output(input);

	if (usb_devices.empty())
		std::cout << "No USB devices found\n";

	auto pci_devices = parse_pcidump_output(pcidump);

	if (pci_devices.empty())
		std::cout << "No PCI devices found\n";

	std::vector<Device> devices;
	devices.reserve(usb_devices.size() + pci_devices.size());
	
	std::ranges::copy(usb_devices, std::back_inserter(devices));
	std::ranges::copy(pci_devices, std::back_inserter(devices));


	auto longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			std::string device_name_a, device_name_b;
			if (a.bus == DeviceBus::USB)
				device_name_a = a.usb_reported_vendor + " " + a.usb_reported_product;
			else if (a.bus == DeviceBus::PCI)
				device_name_a = a.pci_device_name;

			if (b.bus == DeviceBus::USB)
				device_name_b = b.usb_reported_vendor + " " + b.usb_reported_product;
			else if (b.bus == DeviceBus::PCI)
				device_name_b = b.pci_device_name;

			return device_name_a.length() < device_name_b.length();
		});
	size_t max_name_len;
	if (longest->bus == DeviceBus::USB)
		max_name_len = longest->usb_reported_vendor.length() + 1 + longest->usb_reported_product.length();
	else if (longest->bus == DeviceBus::PCI)
		max_name_len = longest->pci_device_name.length();

	longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			return a.address.length() < b.address.length();
		});
	size_t max_addr_len = longest->address.length();


	struct PendingLookup {
		size_t device_index;
		std::future<std::optional<SupportResponse>> future;
	};

	std::vector<PendingLookup> lookups;

	for (size_t i = 0; i < devices.size(); i++) {
		lookups.push_back({
			i,
			std::async(
				std::launch::async,
				lookup_support,
				std::cref(devices[i])
			)
		});
	}

	for (auto& lookup : lookups) {
		const Device& device = devices[lookup.device_index];
		auto support_response = lookup.future.get();

		std::cout << device.controller << "  "; 
		std::cout << std::right << "addr "
			<< std::setw(max_addr_len) << device.address << "  ";
		std::cout << device.vendor_id << ':' << device.product_id << "  ";

		std::string name;
		if (device.bus == DeviceBus::PCI)
			name = device.pci_device_name;
		else if (device.bus == DeviceBus::USB)
			name = device.usb_reported_vendor + " " + device.usb_reported_product;

		std::cout << std::setfill(' ') << std::left <<
			std::setw(max_name_len) << name << "  ";

		std::cout << device.driver << '\n';
		
		if (!support_response)
			continue;
			
		//std::cout << support_response->bus << " ";
		//std::cout << support_response->vendor_id << ":";
		//std::cout << support_response->product_id << '\n';
		for (const SupportMatch& match: support_response->matches) {
			std::cout << "OpenBSD: ";
			std::cout << match.openbsd_vendor << " ";
			std::cout << match.openbsd_device << " ";
			std::cout << "(" << match.openbsd_driver << ")" << '\n';
			if (!match.other_names.empty()) {
				std::cout << "  " << "Also known as\n";
				for (const OtherName& other_name: match.other_names) {
					std::cout << "    " << other_name.vendor;
					std::cout << " " << other_name.device << '\n';
				}
			}
		}
		
	}

	return 0;
}
