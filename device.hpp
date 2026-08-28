#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>

enum class DeviceBus {
	USB,
	PCI
};


struct Device {
	DeviceBus bus;
	std::string controller;
	std::string address;
	std::string vendor_id;
	std::string product_id;
	std::string usb_reported_vendor;
	std::string usb_reported_product;
	std::string pci_device_name;
	std::string driver;
};

#endif
