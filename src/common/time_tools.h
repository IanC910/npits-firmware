
#ifndef TIME_TOOLS_H
#define TIME_TOOLS_H

#include <time.h>

struct timespec get_timespec();

long long get_time_s();
long long get_time_ms();
long long get_time_us();

void sleep_s(long long sleep_time_s);
void sleep_ms(long long sleep_time_ms);
void sleep_us(long long sleep_time_us);

#endif