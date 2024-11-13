#include <iostream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>

using namespace std;

const string gopro_ip = "10.5.5.9";

// Function to perform an HTTP GET request and return the response as a string
string http_get(const string &url) {
    CURL *curl = curl_easy_init();
    string response;
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, +[](char *ptr, size_t size, size_t nmemb, string *data) {
            data->append(ptr, size * nmemb);
            return size * nmemb;
        });
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
    }
    return response;
}

// Function to check if the GoPro is recording
bool is_recording() {
    string status_url = "http://" + gopro_ip + "/gp/gpControl/status";
    string response = http_get(status_url);
    
    Json::CharReaderBuilder builder;
    Json::Value jsonData;
    string errs;
    istringstream ss(response);
    if (Json::parseFromStream(builder, ss, &jsonData, &errs)) {
        return jsonData["status"]["10"].asInt() == 1; // Status ID 10 indicates recording
    }
    return false;
}

// Function to retrieve the latest media file with retries
Json::Value get_latest_media() {
    string media_url = "http://" + gopro_ip + "/gp/gpMediaList";
    for (int i = 0; i < 5; ++i) { // Retry up to 5 times
        string response = http_get(media_url);
        
        Json::CharReaderBuilder builder;
        Json::Value jsonData;
        string errs;
        istringstream ss(response);
        if (Json::parseFromStream(builder, ss, &jsonData, &errs)) {
            if (jsonData["media"].isArray() && !jsonData["media"].empty()) {
                // Access the last element in the "media" array
                return jsonData["media"][jsonData["media"].size() - 1];
            }
        }
        this_thread::sleep_for(chrono::seconds(2));
    }
    return Json::nullValue;
}

// Function to download a file from a URL
bool download_file(const string &url, const string &filename) {
    CURL *curl = curl_easy_init();
    if (curl) {
        FILE *fp = fopen(filename.c_str(), "wb");
        if (!fp) return false;

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

// Function to extract a 10-second HiLight segment using ffmpeg
void extract_hilight_segment(const string &video_path, const string &output_path, int hilight_time) {
    int start_time = max(0, hilight_time - 5);
    string command = "ffmpeg -i " + video_path + " -ss " + to_string(start_time) + " -t 10 -c copy " + output_path;
    system(command.c_str());
    cout << "HiLight segment saved as " << output_path << endl;
}

int main() {
    if (is_recording()) {
        cout << "Recording is ongoing..." << endl;

        // Add HiLight tag
        string hilight_url = "http://" + gopro_ip + "/gp/gpControl/command/storage/tag_moment";
        http_get(hilight_url);
        cout << "HiLight tag added." << endl;

        // Wait for 30 seconds
        this_thread::sleep_for(chrono::seconds(30));

        // Stop recording
        string stop_url = "http://" + gopro_ip + "/gp/gpControl/command/shutter?p=0";
        http_get(stop_url);
        cout << "Recording stopped." << endl;

        // Retrieve the latest media
        Json::Value latest_media = get_latest_media();
        if (!latest_media.isNull()) {
            string folder = latest_media["d"].asString();
            // Access the last element in the "fs" array
            string filename = latest_media["fs"][latest_media["fs"].size() - 1]["n"].asString();
            string video_url = "http://" + gopro_ip + ":8080/videos/DCIM/" + folder + "/" + filename;

            // Download the video
            string video_path = "HiLight_" + filename;
            if (download_file(video_url, video_path)) {
                cout << "Video " << filename << " downloaded successfully!" << endl;

                // Extract HiLight segment
                string status_url = "http://" + gopro_ip + "/gp/gpControl/status";
                Json::Value status_json;
                istringstream ss(http_get(status_url));
                Json::CharReaderBuilder builder;
                string errs;
                Json::parseFromStream(builder, ss, &status_json, &errs);
                
                int hilight_time = status_json["status"]["59"].asInt();
                extract_hilight_segment(video_path, "HiLight_clip.mp4", hilight_time);
            } else {
                cout << "Failed to download video." << endl;
            }
        } else {
            cout << "No media found after retries." << endl;
        }
    } else {
        cout << "GoPro is not recording. Please start recording and try again." << endl;
    }

    return 0;
}
