#ifndef NEAR_PASS_PREDICTOR_H
#define NEAR_PASS_PREDICTOR_H

#include <thread>

#include "../devices/OPS243.h"

class NearPassPredictor {
public:

    struct speed_report_t {
        float speed_mps;
        float magnitude;
    }

    struct range_report_t {
        float range_m;
        float magnitude;
    }
    
    NearPassPredictor(OPS243* radar);
    ~NearPassPredictor();

    // Asynchronous control
    int start();

    int stop();

    // Runs the predictor in a loop until stop() is called. Blocking
    void run();

    bool is_active();

    // Direct control
    void initialize_radar();
    void update_speeds_and_ranges();
    bool is_vehicle_approaching();
    bool is_vehicle_in_range();
    speed_report_t get_speed_of_approaching_vehicle_mps();
    range_report_t get_distance_of_highest_mag_m();

private:
    bool flag;

    OPS243* radar = nullptr;

    float range_m_array[OPS243::MAX_REPORTS];
    float range_magnitude_array[OPS243::MAX_REPORTS];

    float speed_mps_array[OPS243::MAX_REPORTS];
    float speed_magnitude_array[OPS243::MAX_REPORTS];

    bool do_run = false;
    std::thread* predictor_thread = nullptr;
};

#endif