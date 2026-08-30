#include <algorithm>
#include <fstream>
#include <future>
#include <iostream>
#include <iomanip>
#include <ranges>
#include <sstream>
#include "usb_parser.hpp"
#include "pci_parser.hpp"
#include "db_lookup.hpp"


void
usage(const char* program)
{
	std::cerr << "usage: " << program << " [-p pcidump-file]\n";
}

std::string
device_name(const Device& device)
{
	if (const auto* usb = std::get_if<UsbInfo>(&device.bus_info))
		return usb->reported_vendor + " " + usb->reported_product;

	if (const auto* pci = std::get_if<PciInfo>(&device.bus_info))
		return pci->device_name;

	return {};
}

int
main(int argc, char* argv[]) {
	std::string pcidump_path = "";
	
	for (int i = 1; i < argc; i++) {
		std::string flag = argv[i];
		if (flag.length() != 2 || flag[0] != '-') {
			std::cerr << "malformed option flag\n";
			return 1;
		}
		switch (flag[1]) {
			case 'p':
				if (i + 1 >= argc) {
					usage(argv[0]);
					return 1;
				}
				pcidump_path = argv[++i];
				break;
			default:
				usage(argv[0]);
				return 1;
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

	std::vector<Device> pci_devices{};
	if (pcidump_path != "") {

		std::ifstream pcidump_file(pcidump_path);
		if (!pcidump_file.is_open()) {
			std::cerr << "Error: Could not open " << pcidump_path << '\n';
			return 1;
		}

		pci_devices = parse_pcidump_output(pcidump_file);
		if (pci_devices.empty())
			std::cout << "No PCI devices found\n";
	}

	if (!pci_devices.empty()) {

		const std::string dmesg_path = "/var/run/dmesg.boot";

		std::ifstream dmesg_file(dmesg_path);
		if (!dmesg_file.is_open()) {
			std::cerr << "Error: Could not open " << dmesg_path << '\n';
			return 1;
		}

		auto driver_map = map_dmesg_pci_drivers(dmesg_file);
		if (!driver_map)
			return 1;

		for (auto& device: pci_devices) {
			if (driver_map->contains(device.address))
				device.driver = driver_map->at(device.address);
		}
	}


	std::vector<Device> devices;
	devices.reserve(usb_devices.size() + pci_devices.size());
	
	std::ranges::copy(usb_devices, std::back_inserter(devices));
	std::ranges::copy(pci_devices, std::back_inserter(devices));

	if (devices.empty())
		return 0;

	auto longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			return device_name(a).length() < device_name(b).length();
		});
	size_t max_name_len = device_name(*longest).length();

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

		std::cout << std::setfill(' ') << std::left <<
			std::setw(max_name_len) << device_name(device) << "  ";

		std::cout << device.driver << '\n';
		
		if (!support_response)
			continue;
			
		for (const SupportMatch& match: support_response->matches) {
			std::cout << "OpenBSD: ";
			std::cout << match.openbsd_vendor << " ";
			std::cout << match.openbsd_device << " ";
			std::cout << "(" << match.openbsd_driver << ")" << '\n';
			if (!match.other_names.empty()) {
				std::cout << "  " << "Also known as:\n";
				for (const OtherName& other_name: match.other_names) {
					std::cout << "    " << other_name.vendor;
					std::cout << " " << other_name.device << '\n';
				}
			}
		}
		
	}

	return 0;
}
