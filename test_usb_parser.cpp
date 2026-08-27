#include <cassert>
#include <sstream>
#include <vector>

#include "usb_parser.hpp"

int
main() {

	std::istringstream usbdevs(R"(
Controller /dev/usb0:
addr 01: 8086:0000 Intel, xHCI root hub
	 super speed, self powered, config 1, rev 1.00
	 driver: uhub0
addr 02: 0bda:f179 Realtek, 802.11n
	 high speed, power 500 mA, config 1, rev 0.00, iSerial BC1C812385A6
	 driver: urtwn0
addr 03: 0bda:57cc NC2141103L533025AALM04, HD WebCam
	 high speed, power 500 mA, config 1, rev 0.04, iSerial 200901010001
	 driver: uvideo0
addr 04: 2386:0401 Raydium Corporation, Raydium Touch System
	 full speed, power 96 mA, config 1, rev 0.00
	 driver: uhidev0
addr 05: 04ca:3015 Lite-On Technology, product 0x3015
	 full speed, self powered, config 1, rev 0.01
	 driver: ugen0
)");


	std::vector<Device> devices = parse_usbdevs_output(usbdevs);

	assert(devices.size() == 5);

	assert(devices[0].address == 1);
	assert(devices[0].vendor_id == "0x8086");
	assert(devices[0].product_id == "0x0000");
	assert(devices[0].reported_vendor == "Intel");
	assert(devices[0].reported_product == "xHCI root hub");
	assert(devices[0].driver == "uhub0");

	assert(devices[1].address == 2);
	assert(devices[1].vendor_id == "0x0bda");
	assert(devices[1].product_id == "0xf179");
	assert(devices[1].reported_vendor == "Realtek");
	assert(devices[1].reported_product == "802.11n");
	assert(devices[1].driver == "urtwn0");

	assert(devices[2].address == 3);
	assert(devices[2].vendor_id == "0x0bda");
	assert(devices[2].product_id == "0x57cc");
	assert(devices[2].reported_vendor == "NC2141103L533025AALM04");
	assert(devices[2].reported_product == "HD WebCam");
	assert(devices[2].driver == "uvideo0");


	return 0;
}
