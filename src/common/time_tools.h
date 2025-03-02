
#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <time.h>
#include <string>

void set_time_s(time_t time_s);

struct timespec get_timespec_with_offset();

// Returns UNIX time, i.e. time since the epoch, Jan 1, 1970
time_t get_time_s();
time_t get_time_ms();
time_t get_time_us();

std::string get_timestamp_hms();

void sleep_s(time_t sleep_time_s);
void sleep_ms(time_t sleep_time_ms);
void sleep_us(time_t sleep_time_us);

#endif