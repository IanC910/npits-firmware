
#include <stdlib.h>
#include <ctime>
#include <string>
#include <cstring>

#include "time_tools.h"

// Corrective offset added to all get time functions
static time_t global_time_offset_s = 0;

void set_time_s(time_t time_s) {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    global_time_offset_s = time_s - now.tv_sec;
}



struct timespec get_timespec_with_offset() {
    struct timespec now;
    timespec_get(&now, TIME_UTC);
    now.tv_sec += global_time_offset_s;
    return now;
}



time_t get_time_s() {
    struct timespec now = get_timespec_with_offset();
    return now.tv_sec;
}

time_t get_time_ms() {
    struct timespec now = get_timespec_with_offset();
    time_t milliseconds = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    return milliseconds;
}

time_t get_time_us() {
    struct timespec now = get_timespec_with_offset();
    time_t microseconds = now.tv_sec * 1000000 + now.tv_nsec / 1000;
    return microseconds;
}



std::string get_timestamp_hms() {
    struct timespec now = get_timespec_with_offset();
    struct tm* timeinfo = localtime(&now.tv_sec);
    char time_buffer[32];
    memset(time_buffer, 0, sizeof(time_buffer));
    strftime(time_buffer, sizeof(time_buffer), "%H:%M:%S", timeinfo);

    int milliseconds = now.tv_nsec / 1000000;
    char timestamp_cstr[64];
    snprintf(timestamp_cstr, sizeof(timestamp_cstr), "%s.%03d", time_buffer, milliseconds);

    return std::string(timestamp_cstr);
}



void sleep_s(time_t sleep_time_s) {
    struct timespec sleep_time = {sleep_time_s, 0};
    nanosleep(&sleep_time, NULL);
}

void sleep_ms(time_t sleep_time_ms) {
    time_t sleep_time_ns = sleep_time_ms * 1000000;
    int s = sleep_time_ns / 1000000000;
    int ns = sleep_time_ns % 1000000000;
    struct timespec sleep_time = {s, ns};
    nanosleep(&sleep_time, NULL);
}

void sleep_us(time_t sleep_time_us) {
    time_t sleep_time_ns = sleep_time_us * 1000;
    int s = sleep_time_ns / 1000000000;
    int ns = sleep_time_ns % 1000000000;
    struct timespec sleep_time = {s, ns};
    nanosleep(&sleep_time, NULL);
}