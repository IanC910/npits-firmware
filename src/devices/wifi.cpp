#include <iostream>
#include <cstdlib>
#include <cstring>
#include "wifi.h"

#define SSID "GP25425765"

using namespace std;

string gopro_getWiFiSSID() {
    const char* command = "iwgetid -r"; // Command to get the SSID
    char buffer[128];
    string result;

    // Open a pipe to read the command output
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        cerr << "Failed to run command." << endl;
        return "";
    }

    // Read the output of the command
    while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
        result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    // Remove any trailing newline character
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

bool gopro_isConnected() {
    return gopro_getWiFiSSID() == SSID;
}

int main() {
    string ssid = gopro_getWiFiSSID();

    if (!ssid.empty()) {
        cout << "Connected to Wi-Fi network: " << ssid << endl;
    } else {
        cout << "Not connected to any Wi-Fi network or unable to retrieve SSID." << endl;
    }

    if(gopro_isConnected()) {
        cout << "true";
    }
    else {
        cout << "false";
    }

    return 0;
}
