
#ifndef NEAR_PASS_DETECTOR_H
#define NEAR_PASS_DETECTOR_H

#include <string>
#include <thread>

#include "../devices/MB1242.h"

class NearPassDetector {
public:
    NearPassDetector(MB1242* ultrasonic);
    ~NearPassDetector();

    // Returns 0 on success, 1 if ride already active
    int start();

    // Returns 0 on success, 1 if no ride is active
    int stop();

    // Returns true if a ride is currently active, false otherwise
    bool is_active();

    void set_latitude(double latitude);
    void set_longitude(double longitude);
    void set_speed_mps(double speed_mps);

private:
    int curr_ride_id = 0;

    MB1242* ultrasonic = nullptr;

    bool do_run = false;
    std::thread* detector_thread = nullptr;

    double latest_latitude = 0;
    double latest_longitude = 0;
    double latest_speed_mps = 0;

    enum near_pass_state_t {
        NPS_NONE,
        NPS_POTENTIALLY_STARTED,
        NPS_IN_NEAR_PASS,
        NPS_POTENTIALLY_OVER,
    };

    void run();
};

#endif