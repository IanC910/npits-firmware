
#include "../connection_params.h"

#include "../devices/MB1242.h"

#include "../near_pass_detection/NearPassDetector.h"

int main() {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
    NearPassDetector near_pass_detector(&ultrasonic, nullptr);

    // Blocking, no stop condition except ctrl+c
    near_pass_detector.run();
}

