#ifndef GOPRO
#define GOPRO

#include <string>
#include <jsoncpp/json/json.h>
#include <vector>

void start_recording();
void stop_recording();
void add_hilight_tag();
Json::Value get_latest_media();
void process_hilight_clips(const std::string& folder, const std::string& filename);
void post_process_ride();

#endif // GOPRO_CONTROL_H