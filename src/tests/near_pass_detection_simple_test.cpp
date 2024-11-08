
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <queue>

#include "../devices/OPS241.h"
#include "../devices/MB1242.h"



const char RADAR_SERIAL_PORT[] = "/dev/ttyACM0";
const char ULTRASONIC_I2C_DEVICE[] = "/dev/i2c-1";
const int ULTRASONIC_STATUS_GPIO_NUM = 17;



void* radar_thread(void* arg) {
    OPS241 radar(RADAR_SERIAL_PORT);
    const int SLEEP_PERIOD_us = 30000;

    radar.start_reporting_distance();

    double prev_distance_m = 0;
    double prev_speed_mps = 0;

    clock_t start_clock = 0
    while(1) {
        clock_t end_clock = clock();
        double delta_time_s = ((double)(end_clock - start_clock)) / CLOCKS_PER_SEC;
        start_clock = clock();

        delta_time_s += ((double)SLEEP_PERIOD_us) / 1000000;

        float distance_m = radar.read_distance_m();
        if(distance_m == 0) {
            usleep(SLEEP_PERIOD_us);
            continue;
        }

        double delta_distance_m = distance_m - prev_distance_m;
        double speed_mps = delta_distance_m / delta_time_s;

        double delta_speed_mps = speed_mps - prev_speed_mps;
        double acceleration_mps2 = delta_speed_mps / delta_time_s;

        prev_distance_m = distance_m;
        prev_speed_mps = speed_mps;

        usleep(SLEEP_PERIOD_us);
    }
}



void* ultrasonic_thread(void* arg) {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
}



int main() {




}