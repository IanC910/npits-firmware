
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>



// === Connection Params ===

const std::string ULTRASONIC_I2C_DEVICE = "/dev/i2c-1";
const int ULTRASONIC_STATUS_GPIO_NUM = 17;



// === Near Pass Detection Params ===

static const int DISTANCE_THRESHOLD_cm = 200;
static const int NEAR_PASS_COOLDOWN_ms = 300;
static const int NEAR_PASS_MIN_DURATION_ms = 100;
static const int NEAR_PASS_MAX_DURATION_ms = 5000;

#endif