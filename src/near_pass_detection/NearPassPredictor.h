
#ifndef NEAR_PASS_PREDICTOR_H
#define NEAR_PASS_PREDICTOR_H

#include <thread>
#include <queue>

#include "../devices/OPS243.h"

#include "NearPassDetector.h"

class NearPassPredictor {
public:
    struct time_window_t {
        long long start_time_ms;
        long long end_time_ms;
    };

    NearPassPredictor(OPS243* radar, NearPassDetector* near_pass_detector = nullptr);
    ~NearPassPredictor();

    // === Asynchronous control ===

    int start();

    int stop();

    // Runs the predictor in a loop until stop() is called. Blocking
    void run();

    bool is_active();

    // === Direct control ===

    // Configures radar with a preset of settings
    void config_radar();

    OPS243::speed_report_t get_speed_of_highest_mag_mps();
    OPS243::range_report_t get_range_of_highest_mag_m();

    bool is_vehicle_approaching();
    bool is_vehicle_in_range();

    bool is_near_pass_predicted_now();

private:
    long long predicted_start_time_ms = 0;
    long long predicted_end_time_ms = 0;
    std::queue<time_window_t> predictions;

    OPS243* radar = nullptr;

    OPS243::range_report_t range_reports[OPS243::MAX_REPORTS];
    OPS243::speed_report_t speed_reports[OPS243::MAX_REPORTS];

    NearPassDetector* near_pass_detector = nullptr;

    bool do_run = false;
    std::thread* predictor_thread = nullptr;
};

#endif