#ifndef NEAR_PASS_PREDICTOR_H
#define NEAR_PASS_PREDICTOR_H

#include <thread>

#include "../devices/OPS243.h"

class NearPassPredictor {
public:
    NearPassPredictor(OPS243* radar);
    ~NearPassPredictor();

    // Asynchronous control
    int start();

    int stop();

    bool is_active();

    // Direct control
    void initialize_radar();
    void update_speeds_and_ranges();
    bool is_vehicle_approaching();
    bool is_vehicle_in_range();

private:
    bool flag;

    OPS243* radar = nullptr;

    float range_m_array[OPS243::MAX_REPORTS];
    float range_magnitude_array[OPS243::MAX_REPORTS];

    float speed_mps_array[OPS243::MAX_REPORTS];
    float speed_magnitude_array[OPS243::MAX_REPORTS];

    bool do_run = false;
    std::thread* predictor_thread = nullptr;

    void run();
};

#endif