
#include "connection_params.h"

#include "devices/MB1242.h"
#include "devices/OPS243.h"

#include "near_pass_detection/NearPassDetector.h"
#include "near_pass_detection/NearPassPredictor.h"

#include "db/near_pass_db.h"

#include "bluetooth/npits_ble_server.h"

int main() {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
    OPS243 radar(RADAR_SERIAL_PORT);

    NearPassDetector near_pass_detector(&ultrasonic, true);
    NearPassPredictor near_pass_predictor(&radar, &near_pass_detector);

    db_open(NEAR_PASS_DB_FILE);
    db_create_rides_table();
    db_create_near_pass_table();

    npits_ble_server_init(
        LE_SERVER_DEVICES_FILE,
        &near_pass_detector,
        &near_pass_predictor
    );

    npits_ble_server_run();

    return 0;
}