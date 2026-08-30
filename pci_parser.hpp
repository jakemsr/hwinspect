#ifndef PCI_PARSER_HPP
#define PCI_PARSER_HPP

#include <map>
#include <optional>
#include <vector>
#include "device.hpp"


std::map<std::string, std::string>
map_dmesg_pci_drivers(std::istream& dmesg);

std::vector<Device>
parse_pcidump_output(std::istream& pcidump);

#endif
