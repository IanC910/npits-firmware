
#ifndef NEAR_PASS_DETECTOR_H
#define NEAR_PASS_DETECTOR_H

#include <string>
#include <thread>

#include "../devices/MB1242.h"
#include "NearPassPredictor.h"

class NearPassDetector {
public:
    NearPassDetector(MB1242* ultrasonic, bool use_predictor = false);
    ~NearPassDetector();

    // Returns 0 on success, 1 if ride already active
    int start();

    // Returns 0 on success, 1 if no ride is active
    int stop();

    // Runs the detector in a loop until stop() is called. Blocking
    void run();

    // Returns true if a ride is currently active, false otherwise
    bool is_active();

    void set_latitude(double latitude);
    void set_longitude(double longitude);
    void set_cyclist_speed_mps(double cyclist_speed_mps);
    void set_vehicle_speed_mps(double vehicle_speed_mps);
    void set_prediction_flag(bool prediction);

private:
    MB1242* ultrasonic = nullptr;

    bool do_run = false;
    std::thread* detector_thread = nullptr;

    double latest_latitude = 0;
    double latest_longitude = 0;
    double latest_cyclist_speed_mps = 0;
    double latest_vehicle_speed_mps = 0;

    bool use_predictor = false;
    bool is_near_pass_predicted_now = false;

    enum near_pass_state_t {
        NPS_NONE,
        NPS_POTENTIALLY_STARTED,
        NPS_IN_NEAR_PASS,
        NPS_POTENTIALLY_OVER,
    };
};

#endif