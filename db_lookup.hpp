#ifndef DB_LOOKUP_HPP
#define DB_LOOKUP_HPP

#include <string>
#include <vector>
#include <optional>
#include "device.hpp"


struct OtherName {
	std::string vendor;
	std::string device;
};

struct SupportMatch {
	std::string openbsd_vendor;
	std::string openbsd_device;
	std::string openbsd_driver;
	std::vector<OtherName> other_names;
};

struct SupportResponse {
	std::string bus;
	std::string vendor_id;
	std::string product_id;
	std::vector<SupportMatch> matches;
};

std::optional<SupportResponse>
lookup_support(const Device& device);

std::optional<SupportResponse>
parse_support_response(const std::string& input);


#endif
