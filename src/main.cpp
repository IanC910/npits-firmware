
#include <stdlib.h>

#include "connection_params.h"

#include "common/log.h"

#include "devices/MB1242.h"
#include "devices/OPS243.h"

#include "near_pass_detection/NearPassDetector.h"
#include "near_pass_detection/NearPassPredictor.h"

#include "db/near_pass_db.h"

#include "bluetooth/npits_ble_server.h"

int main() {
    MB1242 ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
    OPS243 radar(POSSIBLE_RADAR_SERIAL_PORTS[0]);

    long unsigned port_index = 0;
    while(!radar.is_connected() && port_index < POSSIBLE_RADAR_SERIAL_PORTS.size()) {
        radar.connect_to_port(POSSIBLE_RADAR_SERIAL_PORTS[port_index]);
        port_index++;
    }

    bool radar_is_connected = radar.is_connected();
    log("NPITS", "Radar connected status: " + std::to_string((int)radar_is_connected));

    NearPassDetector near_pass_detector(&ultrasonic, radar_is_connected);
    NearPassPredictor near_pass_predictor(&radar, &near_pass_detector);

    db_open_and_make_tables(NEAR_PASS_DB_FILE);

    npits_ble_server_init(
        LE_SERVER_DEVICES_FILE,
        &near_pass_detector,
        &near_pass_predictor
    );

    // Stops on key press 'x'
    npits_ble_server_run();

    db_close();

    near_pass_detector.stop();
    near_pass_predictor.stop();

    return 0;
}