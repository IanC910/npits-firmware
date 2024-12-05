#ifndef WIFI
#define WIFI

#include <string>

// Function to retrieve the SSID of the connected Wi-Fi network
std::string get_wifi_ssid();

std::string http_get(const std::string &url);

bool download_file(const std::string &url, const std::string &filename);

#endif