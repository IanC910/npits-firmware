
#include <stdio.h>
#include <unistd.h>

#include "../devices/MB1242.h"

int main() {
    const char ULTRASONIC_I2C_DEVICE[] = "/dev/i2c-1";
    const int ULTRASONIC_STATUS_GPIO_NUM = 17;

    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    while(1) {
        int result = ultrasonic.start_distance_reading();
        if(result != 0) {
            printf("Error: Bad write\n");
        }

        while(ultrasonic.is_reading_in_progress()) {
            usleep(10000);
        }

        int distance_cm = ultrasonic.get_distance_report_cm();
        if(distance_cm < 0) {
            printf("Error: Bad read\n");
        }
        else {
            printf("%d cm\n", distance_cm);
        }

        usleep(100000);
    }
}