
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <queue>

#include "../devices/OPS241.h"

// This test is garbage
// TODO: rewrite

typedef struct {
    double time_s;
    double distance_m;
} distance_sample_t;

int main() {
    const char RADAR_SERIAL_PORT[] = "/dev/ttyACM0";
    OPS241 radar(RADAR_SERIAL_PORT);

    radar.stop_reporting_distance();

    char module_info[512];
    int num_bytes = radar.get_module_info(module_info, sizeof(module_info));
    printf("Module Info Size: %d B\n", num_bytes);
    printf(module_info);

    radar.set_num_digits(2);
    radar.start_reporting_distance();

    std::queue<distance_sample_t> distance_sample_queue;
    distance_sample_queue.push({0, 0});
    const int AVG_SAMPLE_COUNT = 4;

    const int SLEEP_PERIOD_us = 65000;

    double time_asleep_since_prev_sample_s = 0;
    double prev_avg_speed_mps = 0;
    clock_t prev_sample_clock = clock();
    while(1) {
        float distance_m = radar.read_distance_m();
        if(distance_m != 0) {
            // Time stamp current sample
            clock_t sample_clock = clock();
            double time_active_since_prev_sample_s = ((double)(sample_clock - prev_sample_clock)) / CLOCKS_PER_SEC;
            double time_since_prev_sample_s = time_active_since_prev_sample_s + time_asleep_since_prev_sample_s;
            double time_s = distance_sample_queue.back().time_s + time_since_prev_sample_s;

            // Compute average speed from current and old sample
            distance_sample_t old_sample = distance_sample_queue.front();
            double delta_time_s = time_s - old_sample.time_s;
            double delta_distance_m = distance_m - old_sample.distance_m;
            double avg_speed_mps = delta_distance_m / delta_time_s;

            double delta_speed_mps = avg_speed_mps - prev_avg_speed_mps;
            double acceleration_mps2 = delta_speed_mps / time_since_prev_sample_s;

            printf("d: %2.2f    s: %2.2f    a: %2.2f\n", distance_m, avg_speed_mps, acceleration_mps2);

            // Push current sample
            distance_sample_t distance_sample;
            distance_sample.time_s = time_s;
            distance_sample.distance_m = distance_m;
            distance_sample_queue.push(distance_sample);
            if(distance_sample_queue.size() > AVG_SAMPLE_COUNT) {
                distance_sample_queue.pop();
            }

            // Store current sample as previous
            prev_sample_clock = sample_clock;
            prev_avg_speed_mps = avg_speed_mps;
            time_asleep_since_prev_sample_s = 0;
        }

        usleep(SLEEP_PERIOD_us);
        time_asleep_since_prev_sample_s += ((double)SLEEP_PERIOD_us) / 1000000;
    }
}