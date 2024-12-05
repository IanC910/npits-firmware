#ifndef WIFI_SSID_H
#define WIFI_SSID_H

#include "wifi.hpp"

#include <string>

// Function to retrieve the SSID of the connected Wi-Fi network
std::string gopro_getWiFiSSID();
std:string gopro_isConnected();

#endif // WIFI_SSID_H