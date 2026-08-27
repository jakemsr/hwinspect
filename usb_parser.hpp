#ifndef USB_PARSER_HPP
#define USB_PARSER_HPP

#include "device.hpp"

#include <optional>
#include <vector>

std::optional<Device>
parse_device_line(const std::string& line);

std::vector<Device>
parse_usbdevs_output(std::istream& input);

std::optional<std::string>
run_usbdevs();

#endif
