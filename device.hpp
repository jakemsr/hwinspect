#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>
#include <variant>


struct UsbInfo {
	std::string reported_vendor;
	std::string reported_product;
};

struct PciInfo {
	std::string device_name;
};

using BusInfo = std::variant<UsbInfo, PciInfo>;

struct Device {
	std::string controller;
	std::string address;
	std::string vendor_id;
	std::string product_id;
	std::string driver;
	BusInfo bus_info;
};

#endif
