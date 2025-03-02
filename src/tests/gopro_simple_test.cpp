#include <iostream>
#include <curl/curl.h>
#include <jsoncpp/json/json.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iomanip>

using namespace std;

const string GOPRO_IP = "10.5.5.9";

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
bool gopro_is_recording() {
    string status_url = "http://" + GOPRO_IP + "/gp/gpControl/status";
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
Json::Value gopro_get_latest_media() {
    string media_url = "http://" + GOPRO_IP + "/gp/gpMediaList";
    for (int i = 0; i < 5; ++i) { // Retry up to 5 times
        string response = http_get(media_url);

        Json::CharReaderBuilder builder;
        Json::Value jsonData;
        string errs;
        istringstream ss(response);
        if (Json::parseFromStream(builder, ss, &jsonData, &errs)) {
            if (jsonData["media"].isArray() && !jsonData["media"].empty()) {
                // Access the last media entry
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

// Function to generate a unique filename based on timestamp
string generate_unique_filename(const string& base_name) {
    auto now = chrono::system_clock::now();
    auto now_c = chrono::system_clock::to_time_t(now);
    char buffer[100];
    strftime(buffer, sizeof(buffer), "%Y%m%d_%H%M%S", localtime(&now_c));
    return base_name + "_" + string(buffer) + ".mp4";
}

// Function to fetch HiLight timestamps from the video metadata
vector<int> get_hilight_timestamps(const string& folder, const string& filename) {
    string media_info_url = "http://" + GOPRO_IP + ":8080/gopro/media/info?path=" + folder + "/" + filename;
    string response = http_get(media_info_url);

    Json::CharReaderBuilder builder;
    Json::Value jsonData;
    string errs;
    istringstream ss(response);
    vector<int> hilight_times;
    if (Json::parseFromStream(builder, ss, &jsonData, &errs)) {
        if (jsonData["hi"].isArray()) {
            for (const auto& time : jsonData["hi"]) {
                hilight_times.push_back(time.asInt());
            }
        }
    }
    return hilight_times;
}

// Function to extract a 10-second HiLight segment using ffmpeg with precise seeking
void gopro_extract_hilight_segment(const string &video_path, int hilight_time_ms) {
    // Convert HiLight time from milliseconds to seconds
    double hilight_time = hilight_time_ms / 1000.0;

    // Calculate the start time for the extraction (5 seconds before the HiLight tag)
    double start_time = max(0.0, hilight_time - 5.0);

    // Generate a unique filename for the output clip
    string output_path = generate_unique_filename("HiLight_clip");

    // Construct the ffmpeg command with accurate seeking
    stringstream command;
    command << "ffmpeg -i " << video_path
            << " -ss " << fixed << setprecision(3) << start_time
            << " -t 10 -c copy -avoid_negative_ts 1 " << output_path;

    // Execute the command and check for success
    int result = system(command.str().c_str());
    if (result == 0) {
        cout << "HiLight segment saved as " << output_path << endl;
    } else {
        cout << "Failed to extract HiLight segment." << endl;
    }
}

int main() {
    if (gopro_is_recording()) {
        cout << "Recording is ongoing..." << endl;

        // Add HiLight tag
        string hilight_url = "http://" + GOPRO_IP + "/gp/gpControl/command/storage/tag_moment";
        http_get(hilight_url);
        cout << "HiLight tag added." << endl;

        // Wait for 5 seconds
        this_thread::sleep_for(chrono::seconds(5));

        // Stop recording
        string stop_url = "http://" + GOPRO_IP + "/gp/gpControl/command/shutter?p=0";
        http_get(stop_url);
        cout << "Recording stopped." << endl;

        // Retrieve the latest media
        Json::Value latest_media = gopro_get_latest_media();
        if (!latest_media.isNull()) {
            string folder = latest_media["d"].asString();
            string filename = latest_media["fs"][latest_media["fs"].size() - 1]["n"].asString();
            string video_url = "http://" + GOPRO_IP + ":8080/videos/DCIM/" + folder + "/" + filename;

            // Download the video
            string video_path = "HiLight_" + filename;
            if (download_file(video_url, video_path)) {
                cout << "Video " << filename << " downloaded successfully!" << endl;

                // Retrieve HiLight tags for the specific media file
                string media_info_url = "http://" + GOPRO_IP + ":8080/gopro/media/info?path=" + folder + "/" + filename;
                string media_info_response = http_get(media_info_url);

                Json::CharReaderBuilder builder;
                Json::Value media_info_json;
                string errs;
                istringstream ss(media_info_response);
                if (Json::parseFromStream(builder, ss, &media_info_json, &errs)) {
                    if (media_info_json.isMember("hi") && media_info_json["hi"].isArray() && !media_info_json["hi"].empty()) {
                        // Get the first HiLight tag timestamp in milliseconds
                        int hilight_time_ms = media_info_json["hi"][0].asInt();

                        // Extract HiLight segment
                        gopro_extract_hilight_segment(video_path, hilight_time_ms);
                    } else {
                        cout << "No HiLight tags found in the media." << endl;
                    }
                } else {
                    cout << "Failed to parse media info JSON." << endl;
                }
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


