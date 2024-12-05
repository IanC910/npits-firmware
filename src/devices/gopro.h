#ifndef GOPRO_H
#define GOPRO_H

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>

const std::string GOPRO_SSID = "GP25425765";

void gopro_start_recording();
void gopro_stop_recording();
void add_hilight_tag();
Json::Value gopro_get_latest_media();
void gopro_process_hilight_clips(const std::string& folder, const std::string& filename);
void gopro_post_process_ride();
bool gopro_isConnected();

#endif // GOPRO_H