
#ifndef NEAR_PASS_DETECTION_H
#define NEAR_PASS_DETECTION_H

#include <string>

void near_pass_detection_init(std::string ultrasonic_i2c_device, int ultrasonic_status_gpio_num);

void near_pass_detection_start();

void near_pass_detection_stop();

#endif