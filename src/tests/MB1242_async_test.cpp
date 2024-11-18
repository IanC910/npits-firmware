
#include <stdio.h>

#include "../common/time_tools.h"

#include "../devices/MB1242.h"

const char ULTRASONIC_I2C_DEVICE[] = "/dev/i2c-1";
const int ULTRASONIC_STATUS_GPIO_NUM = 17;

int main() {

    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    ultrasonic.begin_sampling();

    while(1) {
        while(!ultrasonic.is_new_distance_available()) {
            sleep_ms(5);
        }

        int distance_cm = ultrasonic.get_latest_distance_cm();
        printf("%lld: %3d cm\n", get_time_ms(), distance_cm);
    }
}