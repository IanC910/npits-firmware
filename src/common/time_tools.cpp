
#include <stdlib.h>
#include <time.h>

#include "time_tools.h"

struct timespec get_timespec() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now;
}

long long get_time_s() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec;
}

long long get_time_ms() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    long long milliseconds = now.tv_sec * 1000 + now.tv_nsec / 1000000;
    return milliseconds;
}

long long get_time_us() {
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    long long microseconds = now.tv_sec * 1000000 + now.tv_nsec / 1000;
    return microseconds;
}

void sleep_ms(long long sleep_time_ms) {
    long long sleep_time_ns = sleep_time_ms * 1000000;
    int s = sleep_time_ns / 1000000000;
    int ns = sleep_time_ns % 1000000000;
    struct timespec sleep_time = {s, ns};
    nanosleep(&sleep_time, NULL);
}

void sleep_us(long long sleep_time_us) {
    long long sleep_time_ns = sleep_time_us * 1000;
    int s = sleep_time_ns / 1000000000;
    int ns = sleep_time_ns % 1000000000;
    struct timespec sleep_time = {s, ns};
    nanosleep(&sleep_time, NULL);
}