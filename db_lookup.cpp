#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "db_lookup.hpp"

namespace {

size_t
write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	std::string *response =
	    static_cast<std::string *>(userdata);

	size_t bytes = size * nmemb;

	response->append(ptr, bytes);

	return bytes;
}

}

std::optional<SupportResponse>
lookup_support(const Device& device) {
	
	std::string url = 
		"https://obsd-device-support.vercel.app/"
		"api/devices/by_bus_vid_pid"
		"?bus=USB"
		"&vendor_id=" + device.vendor_id +
		"&product_id=" + device.product_id;

	std::string response;

	CURL *curl = curl_easy_init();

	if (!curl) {
		std::cerr << "Failed to initialize cCURL\n";
		return std::nullopt;
	}
	
	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	CURLcode result = curl_easy_perform(curl);
	
	if (result != CURLE_OK) {
		std::cerr << "curl error: "
			<< curl_easy_strerror(result) << '\n';
		curl_easy_cleanup(curl);
		return std::nullopt;
	}
	
	long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    if (http_status != 200) {
		std::cerr << "HTTP error: "
			<< http_status << '\n';
		curl_easy_cleanup(curl);
		return std::nullopt;
	}

	curl_easy_cleanup(curl);

	return parse_support_response(response);
}


std::optional<SupportResponse>
parse_support_response(const std::string& input) {

	SupportResponse support_response{};
	
	using json = nlohmann::json;

	try {
		json data = json::parse(input);

		support_response.bus = data.at("bus").get<std::string>();
		support_response.vendor_id = data.at("vendor_id").get<std::string>();
		support_response.product_id = data.at("product_id").get<std::string>();

		for (const auto& match : data.at("matches")) {
			SupportMatch support_match{};
			support_match.openbsd_vendor = match.at("vendor").get<std::string>();
			support_match.openbsd_device = match.at("device").get<std::string>();
			support_match.openbsd_driver = match.at("driver").get<std::string>();
			for (const auto& name : match.at("other_names")) {
				OtherName other_name{};
				other_name.vendor = name.at("vendor").get<std::string>();
				other_name.device = name.at("device").get<std::string>();
				support_match.other_names.push_back(other_name);
			}
			support_response.matches.push_back(support_match);
		}
	} catch (const json::parse_error& e) {
		std::cerr << "JSON parse error: " << e.what() << '\n';
		return std::nullopt;
	} catch (const json::type_error& e) {
		std::cerr << "JSON type error: " << e.what() << '\n';
		return std::nullopt;
	} catch (const json::out_of_range& e) {
		std::cerr << "JSON missing field: " << e.what() << '\n';
		return std::nullopt;
	}
	
	return support_response;
}
