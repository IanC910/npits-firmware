#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "../common/time_tools.h"
#include "../common/log.h"

#include "../devices/OPS243.h"

#include "near_pass_prediction_params.h"
#include "NearPassPredictor.h"

NearPassPredictor::NearPassPredictor(OPS243* radar, NearPassDetector* near_pass_detector = nullptr) {
    this->radar = radar;
    this->near_pass_detector = near_pass_detector;

    if(radar == nullptr) {
        log("NearPassPredictor", "Warning, radar is nullptr");
    }

    if(near_pass_detector == nullptr) {
        log("NearPassPredictor", "Warning, near pass detector is nullptr");
    }
}

NearPassPredictor::~NearPassPredictor() {
    stop();
}

int NearPassPredictor::start() {
    if(radar == nullptr) {
        return 1;
    }

    if(predictor_thread != nullptr) {
        return 1;
    }

    predictor_thread = new std::thread(&NearPassPredictor::run, this);

    return 0;
}

int NearPassPredictor::stop() {
    if(predictor_thread == nullptr) {
        return 1;
    }

    do_run = false;
    if(predictor_thread->joinable()) {
        predictor_thread->join();
        delete predictor_thread;
        predictor_thread = nullptr;
    }

    return 0;
}

void NearPassPredictor::run() {
    if(radar == nullptr) {
        log("NearPassPredictor", "Couldn't run, radar is nullptr");
        return;
    }

    if(do_run) {
        log("NearPassPredictor", "Couldn't run, already running");
        return;
    }

    log("NearPassPredictor", "Starting...");

    config_radar();

    do_run = true;
    while(do_run) {
        int option = radar->read_new_data_line(range_reports, speed_reports);

        long long now_ms = get_time_ms();

        if(option == 1) { // Range data
            log("NearPassPredictor", "Radar range data:");
            printf("Range:     ");
            for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
                printf("%16.2f", range_reports[i].range_m);
            }
            printf("\nMagnitude: ");
            for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
                printf("%16.2f", range_reports[i].magnitude);
            }
            printf("\n");

            OPS243::range_report_t range_report = get_range_of_highest_mag_m();
            if(range_report.magnitude != 0) {
                log("NearPassPredictor",
                    std::string("Vehicle in range! ") +
                    std::string(" Range m: ") + std::to_string(range_report.range_m) +
                    std::string(" Magnitude: ") + std::to_string(range_report.magnitude)
                );
            }
        }
        else if(option == 2) { // Speed data updated
            log("NearPassPredictor", "Radar speed data:");
            printf("\nSpeed:     ");
            for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
                printf("%16.2f", speed_reports[i].speed_mps);
            }
            printf("\nMagnitude: ");
            for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
                printf("%16.2f", speed_reports[i].magnitude);
            }
            printf("\n");

            OPS243::speed_report_t speed_report = get_speed_of_highest_mag_mps();
            if(speed_report.magnitude != 0) {
                log("NearPassPredictor",
                    std::string("Vehicle approaching! ") +
                    std::string(" Speed mps: ") + std::to_string(speed_report.speed_mps) +
                    std::string(" Magnitude: ") + std::to_string(speed_report.magnitude)
                );

                // Prediction logic currently assumes no range data!
                // Assumes a range of probable distances and a grace period

                const float MAX_PROBABLE_DISTANCE_m = 20;
                const float MIN_PROBABLE_DISTANCE_m = 5;

                long long min_start_time = now_ms + 1000 * (long long)(MIN_PROBABLE_DISTANCE_m / speed_report.speed_mps);
                long long max_start_time = now_ms + 1000 * (long long)(MAX_PROBABLE_DISTANCE_m / speed_report.speed_mps);

                predicted_start_time_ms = min_start_time;
                predicted_end_time_ms = max_start_time + 2000; // arbitrary 2 second grace

                if(near_pass_detector != nullptr) {
                    near_pass_detector->set_vehicle_speed_mps(speed_report.speed_mps);
                }
            }
        }

        bool prediction_flag = is_near_pass_predicted_now();

        if(prediction_flag) {
            log("NearPassPredictor", "Near pass is predicted now!");
        }

        if(near_pass_detector != nullptr) {
            near_pass_detector->set_prediction_flag(prediction_flag);
        }

        sleep_ms(20);
    }

    radar->turn_range_reporting_off();
    radar->turn_speed_reporting_off();
}

bool NearPassPredictor::is_active() {
    return (predictor_thread != nullptr);
}

void NearPassPredictor::config_radar() {
    if(radar == nullptr) {
        log("NearPassPredictor", "Coudn't initialize radar, radar is nullptr");
        return;
    }

    radar->set_num_range_reports(OPS243::MAX_REPORTS);
    radar->set_num_speed_reports(OPS243::MAX_REPORTS);
    radar->turn_units_output_on();
    radar->turn_fmcw_magnitude_reporting_on();
    radar->turn_doppler_magnitude_reporting_on();
    radar->set_inbound_only();
    radar->enable_peak_speed_average();
    radar->turn_range_reporting_on();
    radar->turn_speed_reporting_on();
}

OPS243::speed_report_t NearPassPredictor::get_speed_of_highest_mag_mps() {
    int highest_mag_index = 0;
    bool valid_report_found = false;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (speed_reports[i].speed_mps >= MIN_SPEED_mps &&
            speed_reports[i].magnitude > MIN_SPEED_MAGNITUDE
        ) {
            valid_report_found = true;

            if(speed_reports[i].magnitude > speed_reports[highest_mag_index].magnitude) {
                highest_mag_index = i;
            }
        }
	}

    if(!valid_report_found) {
        return {0, 0};
    }

    return speed_reports[highest_mag_index];
}

OPS243::range_report_t NearPassPredictor::get_range_of_highest_mag_m() {
    int highest_mag_index = 0;
    bool valid_report_found = false;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (range_reports[i].range_m >= MIN_RANGE_m &&
            range_reports[i].magnitude > MIN_RANGE_MAGNITUDE
        ) {
            valid_report_found = true;

            if(range_reports[i].magnitude > range_reports[highest_mag_index].magnitude) {
                highest_mag_index = i;
            }

        }
    }

    if(!valid_report_found) {
        return {0, 0};
    }

    return range_reports[highest_mag_index];
}

bool NearPassPredictor::is_vehicle_approaching() {
    OPS243::speed_report_t speed_report = get_speed_of_highest_mag_mps();
    return (speed_report.magnitude != 0);
}

bool NearPassPredictor::is_vehicle_in_range() {
    OPS243::range_report_t range_report = get_range_of_highest_mag_m();
    return (range_report.magnitude != 0);
}

bool NearPassPredictor::is_near_pass_predicted_now() {
    long long now_ms = get_time_ms();

    if(now_ms >= predicted_start_time_ms && now_ms <= predicted_end_time_ms) {
        return true;
    }
    return false;
}



