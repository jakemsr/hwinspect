#include <iostream>
#include <sstream>
#include <string>
#include "usb_parser.hpp"



std::optional<Device>
parse_device_line(const std::string& line) {

	Device device{};

	size_t pos = 0;
	size_t end_pos = 0;
	size_t len = 0;

	const std::string addr_delim = "addr ";
	const std::string vid_delim = ": ";
	const std::string pid_delim = ":";
	const std::string vendor_delim = " ";
	const std::string product_delim = ", ";

	/*
	 * address
	 */
	if ((pos = line.find(addr_delim, pos)) == std::string::npos)
		return std::nullopt;

	pos += addr_delim.length();
	end_pos = line.find(vid_delim, pos);
	if (end_pos == std::string::npos)
		return std::nullopt;

	len = end_pos - pos;
	try {
		device.address = std::stoi(line.substr(pos, len));
	} catch (const std::exception&) {
		return std::nullopt;
	}
	pos = end_pos;


	/*
	 * vendor_id
	 */
	if ((pos = line.find(vid_delim, pos)) == std::string::npos)
		return std::nullopt;

	pos += vid_delim.length();
	end_pos = line.find(pid_delim, pos);
	if (end_pos == std::string::npos)
		return std::nullopt;
	
	len = end_pos - pos;
	device.vendor_id = "0x" + line.substr(pos, len);
	pos = end_pos;


	/*
	 * product_id
	 */
	if ((pos = line.find(pid_delim, pos)) == std::string::npos)
		return std::nullopt;

	pos += pid_delim.length();
	end_pos = line.find(vendor_delim, pos);
	if (end_pos == std::string::npos)
		return std::nullopt;

	len = end_pos - pos;
	device.product_id = "0x" + line.substr(pos, len);
	pos = end_pos;


	/*
	 * vendor
	 */
	if ((pos = line.find(vendor_delim, pos)) == std::string::npos)
		return std::nullopt;

	pos += vendor_delim.length();
	end_pos = line.find(product_delim, pos);
	if (end_pos == std::string::npos)
		return std::nullopt;

	len = end_pos - pos;
	device.reported_vendor = line.substr(pos, len);
	pos = end_pos;


	/*
	 * product
	 */
	if ((pos = line.find(product_delim, pos)) == std::string::npos)
		return std::nullopt;

	pos += product_delim.length();
	device.reported_product = line.substr(pos);


	return device;
}

std::vector<Device>
parse_usbdevs_output(std::istream& input) {

	std::vector<Device> devices;
	std::string line;
	const std::string driver_delim = "driver: ";
	const std::string controller_delim = "Controller ";
	std::string controller;
	size_t pos = 0;
	size_t end_pos = 0;

	while (std::getline(input, line)) {
		if (line.starts_with(controller_delim)) {
			end_pos = line.find(":", controller_delim.length());
			controller = line.substr(controller_delim.length(),
				end_pos - controller_delim.length());
		} else if (line.starts_with("addr ")) {
			auto device = parse_device_line(line);
			if (device) {
				device->controller = controller;
				devices.push_back(*device);
			}
		} else if ((pos = line.find(driver_delim)) != std::string::npos) {
			if (!devices.empty()) {
				Device& device = devices.back();
				device.driver = line.substr(pos + driver_delim.length());
			}
		}
	}

	return devices;
}

std::optional<std::string>
run_usbdevs() {
	char buffer[1024];
	std::string output;

	FILE *pipe = popen("usbdevs -v", "r");
	if (!pipe)
		return std::nullopt;

	while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
		output += buffer;

	int status = pclose(pipe);
	if (status != 0)
		return std::nullopt;

	return output;
}
