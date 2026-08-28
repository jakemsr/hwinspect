#ifndef PCI_PARSER_HPP
#define PCI_PARSER_HPP

#include "device.hpp"

#include <optional>
#include <vector>

std::vector<Device>
parse_pcidump_output(const std::string& input);

#endif
