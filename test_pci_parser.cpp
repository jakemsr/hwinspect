#include <cassert>
#include <sstream>
#include <vector>

#include "pci_parser.hpp"

int
main() {
	std::istringstream pcidump(R"(
Domain /dev/pci0:
 0:0:0: Intel Braswell Host
	0x0000: Vendor ID: 8086, Product ID: 2280
	0x0004: Command: 0007, Status: 0000
	0x0008:	Class: 06 Bridge, Subclass: 00 Host,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 00
 0:2:0: Intel HD Graphics
	0x0000: Vendor ID: 8086, Product ID: 22b1
	0x0004: Command: 0007, Status: 0010
	0x0008:	Class: 03 Display, Subclass: 00 VGA,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 00
 0:11:0: Intel Braswell Power
	0x0000: Vendor ID: 8086, Product ID: 22dc
	0x0004: Command: 0007, Status: 0010
	0x0008:	Class: 11 DASP, Subclass: 80 Miscellaneous,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 00
 0:20:0: Intel Braswell xHCI
	0x0000: Vendor ID: 8086, Product ID: 22b5
	0x0004: Command: 0006, Status: 0290
	0x0008:	Class: 0c Serial Bus, Subclass: 03 USB,
		Interface: 30, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 00
 0:26:0: Intel Braswell TXE
	0x0000: Vendor ID: 8086, Product ID: 2298
	0x0004: Command: 0106, Status: 0010
	0x0008:	Class: 10 Crypto, Subclass: 80 Miscellaneous,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 10
 0:27:0: Intel Braswell HD Audio
	0x0000: Vendor ID: 8086, Product ID: 2284
	0x0004: Command: 0006, Status: 0010
	0x0008:	Class: 04 Multimedia, Subclass: 03 HD Audio,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 10
 0:28:0: Intel Braswell PCIE
	0x0000: Vendor ID: 8086, Product ID: 22c8
	0x0004: Command: 0007, Status: 0010
	0x0008:	Class: 06 Bridge, Subclass: 04 PCI,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 81, Latency Timer: 00,
		Cache Line Size: 10
 0:28:1: Intel Braswell PCIE
	0x0000: Vendor ID: 8086, Product ID: 22ca
	0x0004: Command: 0007, Status: 0010
	0x0008:	Class: 06 Bridge, Subclass: 04 PCI,
		Interface: 00, Revision: 21
	0x000c: BIST: 00, Header Type: 81, Latency Timer: 00,
		Cache Line Size: 10
	0x0010: BAR empty (00000000)
	0x0014: BAR empty (00000000)
	0x0018: Primary Bus: 0, Secondary Bus: 2, Subordinate Bus: 2,
		Secondary Latency Timer: 00
	0x001c: I/O Base: f0, I/O Limit: 00, Secondary Status: 2000
	0x0020: Memory Base: 9100, Memory Limit: 9110
		Memory Window: 0x91000000-0x911fffff
	0x0024: Prefetch Memory Base: fff1, Prefetch Memory Limit: 0001
	0x0028: Prefetch Memory Base Upper 32 Bits: ffffffff
	0x002c: Prefetch Memory Limit Upper 32 Bits: 00000000
	0x0030: I/O Base Upper 16 Bits: 0000, I/O Limit Upper 16 Bits: 0000
	0x0038: Expansion ROM Base Address: 00000000
	0x003c: Interrupt Pin: 02, Line: 07, Bridge Control: 0000
	0x0040: Capability 0x10: PCI Express
		Max Payload Size: 128 / 128 bytes
		Max Read Request Size: 128 bytes
		Link Speed: 2.5 / 5.0 GT/s
		Link Width: x1 / x1
		ASPM: L0s, L1 (L1 enabled)
	0x0100: Enhanced Capability 0x00: Unknown
	0x0080: Capability 0x05: Message Signalled Interrupts (MSI)
		Enabled: yes; 1 vectors (1 enabled)
	0x0090: Capability 0x0d: PCI-PCI
	0x00a0: Capability 0x01: Power Management
		State: D0
 1:0:0: Realtek 8168
	0x0000: Vendor ID: 10ec, Product ID: 8168
	0x0004: Command: 0007, Status: 0010
	0x0008:	Class: 02 Network, Subclass: 00 Ethernet,
		Interface: 00, Revision: 15
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 10
	0x0010: BAR io addr: 0x00001000/0x0100
	0x0014: BAR empty (00000000)
	0x0018: BAR mem 64bit addr: 0x0000000091204000/0x00001000
	0x0020: BAR mem 64bit addr: 0x0000000091200000/0x00004000
	0x0028: Cardbus CIS: 00000000
	0x002c: Subsystem Vendor ID: 1025 Product ID: 1022
	0x0030: Expansion ROM Base Address: 00000000
	0x0038: 00000000
	0x003c: Interrupt Pin: 01 Line: 07 Min Gnt: 00 Max Lat: 00
	0x0040: Capability 0x01: Power Management
		State: D0 PME# enabled
	0x0050: Capability 0x05: Message Signalled Interrupts (MSI)
		Enabled: yes; 1 vectors (1 enabled)
	0x0070: Capability 0x10: PCI Express
		Max Payload Size: 128 / 128 bytes
		Max Read Request Size: 512 bytes
		Link Speed: 2.5 / 2.5 GT/s
		Link Width: x1 / x1
		ASPM: L0s, L1 (disabled)
	0x0100: Enhanced Capability 0x01: Advanced Error Reporting
	0x0140: Enhanced Capability 0x02: Virtual Channel Capability
	0x0160: Enhanced Capability 0x03: Device Serial Number
		Serial Number: 01000000684ce000
	0x0170: Enhanced Capability 0x18: Latency Tolerance Reporting
	0x0178: Enhanced Capability 0x1e: L1 PM
	0x00b0: Capability 0x11: Extended Message Signalled Interrupts (MSI-X)
		Enabled: no; table size 4 (BAR 4:0)
 2:0:0: Atheros QCA9377
	0x0000: Vendor ID: 168c, Product ID: 0042
	0x0004: Command: 0006, Status: 0010
	0x0008:	Class: 02 Network, Subclass: 80 Miscellaneous,
		Interface: 00, Revision: 30
	0x000c: BIST: 00, Header Type: 00, Latency Timer: 00,
		Cache Line Size: 10
	0x0010: BAR mem 64bit addr: 0x0000000091000000/0x00200000
	0x0018: BAR empty (00000000)
	0x001c: BAR empty (00000000)
	0x0020: BAR empty (00000000)
	0x0024: BAR empty (00000000)
	0x0028: Cardbus CIS: 00000000
	0x002c: Subsystem Vendor ID: 11ad Product ID: 0806
	0x0030: Expansion ROM Base Address: 00000000
	0x0038: 00000000
	0x003c: Interrupt Pin: 01 Line: 07 Min Gnt: 00 Max Lat: 00
	0x0040: Capability 0x01: Power Management
		State: D0
	0x0050: Capability 0x05: Message Signalled Interrupts (MSI)
		Enabled: no; 8 vectors (1 enabled)
	0x0070: Capability 0x10: PCI Express
		Max Payload Size: 128 / 256 bytes
		Max Read Request Size: 512 bytes
		Link Speed: 2.5 / 2.5 GT/s
		Link Width: x1 / x1
		ASPM: L0s, L1 (L1 enabled)
	0x0100: Enhanced Capability 0x01: Advanced Error Reporting
	0x0148: Enhanced Capability 0x02: Virtual Channel Capability
	0x0168: Enhanced Capability 0x03: Device Serial Number
		Serial Number: 0000000000000000
	0x0178: Enhanced Capability 0x18: Latency Tolerance Reporting
	0x0180: Enhanced Capability 0x1e: L1 PM
)");

	std::vector<std::string> dmesg_lines = {
R"(iosf0 at pci0 dev 0 function 0 "Intel Braswell Host" rev 0x21: mbi)",
R"(inteldrm0 at pci0 dev 2 function 0 "Intel HD Graphics" rev 0x21)",
R"("Intel Braswell Power" rev 0x21 at pci0 dev 11 function 0 not configured)",
R"(xhci0 at pci0 dev 20 function 0 "Intel Braswell xHCI" rev 0x21: msi, xHCI 1.0)",
R"("Intel Braswell TXE" rev 0x21 at pci0 dev 26 function 0 not configured)",
R"(azalia0 at pci0 dev 27 function 0 "Intel Braswell HD Audio" rev 0x21: msi)",
R"(ppb0 at pci0 dev 28 function 0 "Intel Braswell PCIE" rev 0x21: msi)",
R"(re0 at pci1 dev 0 function 0 "Realtek 8168" rev 0x15: RTL8168H/8111H (0x5400), msi, address 30:65:ec:8c:84:cc)",
R"(ppb1 at pci0 dev 28 function 1 "Intel Braswell PCIE" rev 0x21: msi)",
R"("Atheros QCA9377" rev 0x30 at pci2 dev 0 function 0 not configured)",
R"(pcib0 at pci0 dev 31 function 0 "Intel Braswell PCU LPC" rev 0x21)",
R"(ichiic0 at pci0 dev 31 function 3 "Intel Braswell SMBus" rev 0x21: apic 1 int 18)"
};

	std::vector<Device> devices = parse_pcidump_output(pcidump, dmesg_lines);

	assert(devices.size() == 10);

 	assert(devices[0].address == "0:0:0");
	assert(devices[0].vendor_id == "0x8086");
	assert(devices[0].product_id == "0x2280");
	assert(std::get_if<PciInfo>(&devices[0].bus_info)->device_name == "Intel Braswell Host");
	assert(devices[0].driver == "iosf0");

	assert(devices[5].address == "0:27:0");
	assert(devices[5].vendor_id == "0x8086");
	assert(devices[5].product_id == "0x2284");
	assert(std::get_if<PciInfo>(&devices[5].bus_info)->device_name == "Intel Braswell HD Audio");
	assert(devices[5].driver == "azalia0");

	assert(devices[8].address == "1:0:0");
	assert(devices[8].vendor_id == "0x10ec");
	assert(devices[8].product_id == "0x8168");
	assert(std::get_if<PciInfo>(&devices[5].bus_info)->device_name == "Realtek 8168");
	assert(devices[8].driver == "re0");


	return 0;
}
