#ifndef PCI_PARSER_HPP
#define PCI_PARSER_HPP

#include "device.hpp"

#include <optional>
#include <vector>

std::vector<std::string>
get_dmesg_pci_lines();

std::vector<Device>
parse_pcidump_output(std::istream& pcidump,
	const std::vector<std::string>& dmesg_lines);

#endif
