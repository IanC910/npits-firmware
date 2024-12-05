#include <iostream>
#include <cstdlib>
#include <cstring>

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include "wifi.h"

using namespace std;

string get_wifi_ssid() {
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

string http_get(const string &url)
{
    CURL *curl = curl_easy_init();
    string response;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
            +[](char *ptr, size_t size, size_t nmemb, string *data) {
                data->append(ptr, size * nmemb);
                return size * nmemb;
            }
        );
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

bool download_file(const string &url, const string &filename)
{
    CURL *curl = curl_easy_init();
    if (curl)
    {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (!fp)
            return false;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_FAILONERROR, 1L);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);
        return true;
    }
    return false;
}
