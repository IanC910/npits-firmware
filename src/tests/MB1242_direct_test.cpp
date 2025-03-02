
#include <stdio.h>
#include <string>

#include "../common/time_tools.h"

#include "../devices/MB1242.h"

const string ULTRASONIC_I2C_DEVICE = "/dev/i2c-1";
const int ULTRASONIC_STATUS_GPIO_NUM = 17;

int main() {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    while(1) {
        int result = ultrasonic.initiate_distance_reading();
        if(result != 0) {
            printf("Error: Bad write\n");
        }

        while(ultrasonic.is_reading_in_progress()) {
            sleep_ms(5);
        }

        result = ultrasonic.update_report();
        if(result == 0) {
            MB1242::report latest_report = ultrasonic.get_latest_report();
            printf("%lld: %3d cm\n",
                latest_report.time_stamp_ms,
                latest_report.distance_cm
            );
        }
        else {
            printf("Error: Couldn't update report\n");
        }
    }
}