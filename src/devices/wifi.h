#ifndef WIFI
#define WIFI

#include <string>

// Function to retrieve the SSID of the connected Wi-Fi network
std::string gopro_getWiFiSSID();
std::bool gopro_isConnected();

#endif