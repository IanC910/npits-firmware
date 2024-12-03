
#include <string>
#include <thread>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include "../connection_params.h"

#include "../common/keyboard_tools.h"

#include "../devices/MB1242.h"
#include "../devices/OPS243.h"

#include "../near_pass_detection/NearPassPredictor.h"
#include "../near_pass_detection/NearPassDetector.h"



int main() {
    OPS243 radar(RADAR_SERIAL_PORT);
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    NearPassPredictor near_pass_predictor(&radar);
    NearPassDetector near_pass_detector(&ultrasonic, &near_pass_predictor);

    // Start this one asynchronously on its own thread
    near_pass_predictor.start();

    // Run this one here, blocking. No stop condition except ctrl+c
    near_pass_detector.start();

    wait_for_key_press('q');

    near_pass_detector.stop();
    near_pass_predictor.stop();


}