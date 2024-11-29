
#ifndef NPITS_BLE_SERVER_H
#define NPITS_BLE_SERVER_H

#include "../near_pass_detection/NearPassDetector.h"
#include "../near_pass_detection/NearPassPredictor.h"

void npits_ble_server_init(
    std::string devices_file,
    NearPassDetector* near_pass_detector,
    NearPassPredictor* near_pass_predictor
);

void npits_ble_server_start();

// Stops the BLE server. Non-blocking
void npits_ble_server_stop();

// Runs the BLE server. Blocking
void npits_ble_server_run();

#endif