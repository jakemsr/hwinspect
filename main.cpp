#include <algorithm>
#include <future>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "usb_parser.hpp"
#include "db_lookup.hpp"


int
main() {
	auto output = run_usbdevs();
	if (!output) {
		std::cerr << "Failed to read usbdevs\n";
		return 1;
	}

	std::istringstream input(*output);

	auto devices = parse_usbdevs_output(input);

	if (devices.empty()) {
		std::cout << "No USB devices found\n";
		return 0;
	}

	auto longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			return a.reported_vendor.length() < b.reported_vendor.length();
		});
	size_t max_vendor_len = longest->reported_vendor.length();

	longest = std::max_element(
		devices.begin(),
		devices.end(),
		[](const Device& a, const Device& b) {
			return a.reported_product.length() < b.reported_product.length();
		});
	size_t max_product_len = longest->reported_product.length();

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
		std::cout << std::setfill('0') << std::dec << std::right << "addr " << std::setw(2) << device.address << "  ";
		std::cout << device.vendor_id << ':' << device.product_id << "  ";
		std::cout << std::setfill(' ') << std::left <<
			std::setw(max_vendor_len) <<  device.reported_vendor << "  ";
		std::cout << std::setw(max_product_len) << device.reported_product << "  ";
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
