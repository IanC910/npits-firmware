
#include <stdio.h>

#include "../common/time_tools.h"

#include "../devices/MB1242.h"

int main() {
    const char ULTRASONIC_I2C_DEVICE[] = "/dev/i2c-1";
    const int ULTRASONIC_STATUS_GPIO_NUM = 17;

    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    while(1) {
        int result = ultrasonic.initiate_distance_reading();
        if(result != 0) {
            printf("Error: Bad write\n");
        }

        while(ultrasonic.is_reading_in_progress()) {
            sleep_ms(5);
        }

        ultrasonic.update_distance_report();
        int distance_cm = ultrasonic.get_latest_distance_cm();
        if(distance_cm < 0) {
            printf("Error: Bad read\n");
        }
        else {
            printf("%lld: %3d cm\n", get_time_ms(), distance_cm);
        }
    }
}