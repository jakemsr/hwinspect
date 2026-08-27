#ifndef DEVICE_HPP
#define DEVICE_HPP

#include <string>


struct Device {
	std::string controller;
	int address;
	std::string vendor_id;
	std::string product_id;
	std::string reported_vendor;
	std::string reported_product;
	std::string driver;
};


#endif
