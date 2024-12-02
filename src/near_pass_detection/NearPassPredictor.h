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

    // Runs the predictor in a loop until stop() is called. Blocking
    void run();

    bool is_active();

    // Direct control
    void initialize_radar();
    void update_speeds_and_ranges();
    bool is_vehicle_approaching();
    bool is_vehicle_in_range();
    OPS243::speed_report_t get_speed_of_highest_mag_mps();
    OPS243::range_report_t get_range_of_highest_mag_m();

private:
    bool flag;

    OPS243* radar = nullptr;

    OPS243::range_report_t range_reports[OPS243::MAX_REPORTS];
    OPS243::speed_report_t speed_reports[OPS243::MAX_REPORTS];

    bool do_run = false;
    std::thread* predictor_thread = nullptr;
};

#endif