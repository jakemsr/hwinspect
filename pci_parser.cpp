#include <fstream>
#include <iostream>
#include <regex>
#include "pci_parser.hpp"


namespace {

std::optional<Device>
get_device(const std::string& line, const std::string& next_line) {
	Device device{};

    const std::string name_delim = ": ";
    const std::string vendor_delim = "Vendor ID: ";
    const std::string product_delim = "Product ID: ";
    const int id_len = 4;
	std::size_t end_pos = 0;
	std::size_t pos = 1;

	if ((end_pos = line.find(name_delim)) == std::string::npos) {
		std::cerr << "Could not find name: " << line << '\n';
		return std::nullopt;
	}
	device.address = line.substr(pos, end_pos - pos);

	pos = end_pos + name_delim.length();
	device.bus_info = PciInfo {
		.device_name = line.substr(pos)
	};

	if (!next_line.starts_with("\t0x0000:")) {
		std::cerr << "Could not find vendor/product IDs\n"
			<< "Did you forget -v for pcidump?\n";
		return std::nullopt;
	}
	pos = 0;
	if ((pos = next_line.find(vendor_delim)) == std::string::npos) {
		std::cerr << "Could not find vendor ID\n";
		return std::nullopt;
	}

	pos += vendor_delim.length();
	device.vendor_id = "0x" + next_line.substr(pos, id_len);
	pos += id_len;

	if ((pos = next_line.find(product_delim, pos)) == std::string::npos) {
		std::cerr << "Could not find product ID\n";
		return std::nullopt;
	}

	pos += product_delim.length();
	device.product_id = "0x" + next_line.substr(pos, id_len);

	return device;
}

}

std::optional<std::map<std::string, std::string>>
map_dmesg_pci_drivers(std::istream& dmesg) {
	std::map<std::string, std::string> driver_map;

    const std::regex pattern(R"(^([a-z]+\d+) at pci(\d+) dev (\d+) function (\d+))");
    std::smatch match;
    std::string driver;
    std::string address;
    std::string line;
    while (std::getline(dmesg, line)) {
		if (std::regex_search(line, match, pattern)) {
			driver = match[1].str();
			address = match[2].str() + ":" + match[3].str() + ":" + match[4].str();
			driver_map.try_emplace(address, driver);
		}
	}

    return driver_map;
}

std::vector<Device>
parse_pcidump_output(std::istream& pcidump) {
	std::vector<Device> devices{};

    std::string controller;
    const std::string domain_delim = "Domain ";
    std::string line, next_line;
    std::size_t pos = 0;
    std::size_t end_pos = 0;
    while (std::getline(pcidump, line)) {
		if (line.starts_with(domain_delim)) {
			pos = domain_delim.length();
			if ((end_pos = line.find(":", pos)) != std::string::npos) {
				controller = line.substr(pos, end_pos - pos);
			}
		}
		if (line.starts_with(' ')) {

			if (!std::getline(pcidump, next_line)) {
				std::cerr << "Could not get next line\n";
				continue;
			}

			auto device = get_device(line, next_line);
			if (!device)
				continue;

			device->controller = controller;

			devices.push_back(*device);
		}
    }

    return devices;
}
