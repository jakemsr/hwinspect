#include <fstream>
#include <iostream>
#include <regex>

#include "pci_parser.hpp"

namespace {

std::vector<std::string>
get_dmesg_pci_lines() {
	std::vector<std::string> lines{};

	const std::string dmesg = "/var/run/dmesg.boot";

	std::ifstream file(dmesg);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << dmesg << '\n';
        return lines;
    }

    const std::regex pattern(R"(^[a-z]+\d+ at pci\d+ dev \d+ function \d+)");
    std::string line;
    while (std::getline(file, line)) {
		if (std::regex_search(line, pattern))
			lines.push_back(line);
	}

    return lines;
}

std::string
find_driver(const std::string& address, const std::vector<std::string>& dmesg_lines) {
	std::string driver = "";

    std::string bus, dev, function;
    std::string delim = ":";
    std::size_t pos = 0;
    std::size_t end_pos = 0;
    if ((end_pos = address.find(delim, pos)) == std::string::npos) {
		std::cerr << "Could not find pci bus: " << address << '\n';
		return driver;
	}
	bus = address.substr(pos, end_pos - pos);
	pos = end_pos + delim.length();

    if ((end_pos = address.find(delim, pos)) == std::string::npos) {
		std::cerr << "Could not find pci dev: " << address << '\n';
		return driver;
	}
	dev = address.substr(pos, end_pos - pos);
	pos = end_pos + delim.length();

	function = address.substr(pos);

	std::string bus_dev_func =
		"at pci" + bus + " dev " + dev + " function " + function;

	for (auto line: dmesg_lines) {
		if (line.find(bus_dev_func) != std::string::npos) {
			end_pos = line.find(" ", 0);
			driver = line.substr(0, end_pos);
			break;
		}
	}

	return driver;
}

}

std::vector<Device>
parse_pcidump_output(const std::string& input) {
	std::vector<Device> devices{};

	std::vector<std::string> dmesg_lines = get_dmesg_pci_lines();

	std::ifstream file(input);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open " << input << '\n';
        return devices;
    }

    std::string controller;
    const std::string domain_delim = "Domain ";
    const std::string name_delim = ": ";
    const std::string vendor_delim = "Vendor ID: ";
    const std::string product_delim = "Product ID: ";
    const int id_len = 4;
    std::string line;
    std::size_t pos = 0;
    std::size_t end_pos = 0;
    while (std::getline(file, line)) {
		if (line.starts_with(domain_delim)) {
			pos = domain_delim.length();
			if ((end_pos = line.find(":", pos)) != std::string::npos) {
				controller = line.substr(pos, end_pos - pos);
			}
		}
		if (line.starts_with(' ')) {
			Device device{};
			device.bus = DeviceBus::PCI;
			device.controller = controller;

			pos = 1;
			if ((end_pos = line.find(name_delim)) == std::string::npos) {
				std::cerr << "Could not find name: " << line << '\n';
			}
			device.address = line.substr(pos, end_pos - pos);

			pos = end_pos + name_delim.length();
			device.pci_device_name = line.substr(pos);

			if (!std::getline(file, line)) {
				std::cerr << "Could not get device info\n";
				break;
			}
			if (!line.starts_with("\t0x0000:")) {
				std::cerr << "Could not find vendor/product IDs\n"
					<< "Did you forget -v for pcidump?\n";
				break;
			}
			pos = 0;
			if ((pos = line.find(vendor_delim)) != std::string::npos) {
				pos += vendor_delim.length();
				device.vendor_id = "0x" + line.substr(pos, id_len);
				pos += id_len;
			}
			if ((pos = line.find(product_delim, pos)) != std::string::npos) {
				pos += product_delim.length();
				device.product_id = "0x" + line.substr(pos, id_len);
			}

			device.driver = find_driver(device.address, dmesg_lines);

			devices.push_back(device);
		}
    }

    return devices;
}
