
#include <string>
#include <thread>

#include "../connection_params.h"

#include "../devices/MB1242.h"
#include "../devices/OPS243.h"

#include "NearPassPredictor.cpp"
#include "NearPassDetector.cpp"


int main() {
    OPS243 radar(RADAR_SERIAL_PORT);
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    NearPassPredictor near_pass_predictor(&radar);
    NearPassDetector near_pass_detector(&ultrasonic, &near_pass_predictor);

    // Start this one asynchronously on its own thread
    near_pass_predictor.start();

    // Run this one here, blocking. No stop condition except ctrl+c
    near_pass_detector.run();
}