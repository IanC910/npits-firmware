
#include "connection_params.h"

#include "devices/MB1242.h"

#include "near_pass_detection/NearPassDetector.h"

#include "bluetooth/npits_ble_server.h"

int main() {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
    NearPassDetector near_pass_detector(&ultrasonic);

    npits_ble_server_init(
        LE_SERVER_DEVICES_FILE,
        &near_pass_detector,
        nullptr
    );

    npits_ble_server_run();

    return 0;
}