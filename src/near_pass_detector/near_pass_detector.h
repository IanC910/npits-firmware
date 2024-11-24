
#ifndef NEAR_PASS_DETECTOR_H
#define NEAR_PASS_DETECTOR_H

#include <string>
#include <thread>

#include "../devices/MB1242.h"

class NearPassDetector {
public:
    NearPassDetector();

    // Returns 0 on success, 1 if ride already active
    int start_ride();

    // Returns 0 on success, 1 if no ride is active
    int end_ride();

    // Returns true if a ride is currently active, false otherwise
    bool get_is_ride_active();

    void set_latitude(double latitude);
    void set_longitude(double longitude);
    void set_speed_mps(double speed_mps);

private:
    bool is_ride_active = false;
    int curr_ride_id = 0;

    MB1242 ultrasonic;

    bool do_run_near_pass_detector = false;
    std::thread* detector_thread;

    double latest_latitude = 0;
    double latest_longitude = 0;
    double latest_speed_mps = 0;

    void run_near_pass_detector();
};

#endif