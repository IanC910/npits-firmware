
#ifndef CONNECTION_PARAMS_H
#define CONNECTION_PARAMS_H

#include <string>
#include <vector>

const std::string LE_SERVER_DEVICES_FILE = "devices.txt";

const std::string ULTRASONIC_I2C_DEVICE = "/dev/i2c-1";
const int ULTRASONIC_STATUS_GPIO_NUM = 17;

const std::vector<std::string> POSSIBLE_RADAR_SERIAL_PORTS {
    "/dev/ttyACM0",
    "/dev/ttyACM1",
    "/dev/ttyACM2",
    "/dev/ttyACM3",
    "/dev/ttyACM4"
};

const std::string NEAR_PASS_DB_FILE = "near_pass.db";

#endif