#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "../common/log.h"

#include "../devices/OPS243.h"

#include "near_pass_prediction_params.h"
#include "NearPassPredictor.h"

NearPassPredictor::NearPassPredictor(OPS243* radar) {
    this->radar = radar;

    if(radar == nullptr) {
        log("NearPassPredictor", "Warning, radar is nullptr");
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

    log("NearPassPredictor", "Starting...");

    initialize_radar();

    do_run = true;
    while(do_run) {
        update_speeds_and_ranges();

        log("NearPassPredictor", "Radar data:");
        printf("Range:     ");
        for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
            printf("%8.2f", range_reports[i].range_m);
        }
        printf("\nMagnitude: ");
        for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
            printf("%8.2f", range_reports[i].magnitude);
        }
        printf("\nSpeed:     ");
        for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
            printf("%8.2f", speed_reports[i].speed_mps);
        }
        printf("\nMagnitude: ");
        for(int i = 0; i < OPS243::MAX_REPORTS; i++) {
            printf("%8.2f", speed_reports[i].magnitude);
        }
        printf("\n");

        OPS243::speed_report_t speed_report = get_speed_of_highest_mag_mps();
        if(speed_report != {0, 0}) {
            log("NearPassPredictor",
                std::string("Vehicle approaching! ") + 
                std::string(" Speed mps: ") + std::to_string(speed_report.speed_mps) +
                std::string(" Magnitude: ") + std::to_string(speed_report.magnitude)
            );
        }
            
        OPS243::range_report_t range_report = get_range_of_highest_mag_m();
        if(range_report != {0, 0}) {
            log("NearPassPredictor",
                std::string("Vehicle in range! ") + 
                std::string(" Range m: ") + std::to_string(range_report.range_m) +
                std::string(" Magnitude: ") + std::to_string(range_report.magnitude)
            );
        }
    }

    radar->turn_range_reporting_off();
    radar->turn_speed_reporting_off();
}

bool NearPassPredictor::is_active() {
    return (predictor_thread != nullptr);
}

void NearPassPredictor::initialize_radar() {
    if(radar == nullptr) {
        log("NearPassPredictor", "Coudn't initialize radar, radar is nullptr");
        return;
    }

    radar->set_number_of_range_reports(9);
    radar->set_number_of_speed_reports(9);
    radar->turn_units_output_on();
    radar->turn_fmcw_magnitude_reporting_on();
    radar->turn_doppler_magnitude_reporting_on();
    radar->set_inbound_only();
    radar->set_maximum_range_filter(MAXIMUM_RANGE_VALUE);
    radar->set_minimum_speed_filter(MINIMUM_SPEED_THRESHOLD);
    radar->enable_peak_speed_average();
    radar->turn_range_reporting_on();
    radar->turn_speed_reporting_on();
}

void NearPassPredictor::update_speeds_and_ranges() {
    if(radar == nullptr) {
        log("NearPassPredictor", "Coudn't update, radar is nullptr");
        return;
    }

    radar->read_speeds_and_ranges(
        range_reports,
        speed_reports
    );
}

bool NearPassPredictor::is_vehicle_approaching() {
    OPS243::speed_report_t speed_report = get_speed_of_highest_mag_mps();
    return (speed_report != {0, 0});
}

bool NearPassPredictor::is_vehicle_in_range() {
    OPS243::range_report_t range_report = get_range_of_highest_mag_m();
    return (range_report != {0, 0});
}

OPS243::speed_report_t NearPassPredictor::get_speed_of_highest_mag_mps() {
    int highest_mag_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (speed_reports[i].speed_mps != 0 && speed_reports[i].speed_mps >= MINIMUM_SPEED_THRESHOLD) {
            if (speed_reports[i].magnitude > SPEED_MAGNITUDE_THRESHOLD && speed_reports[i].magnitude > speed_reports[highest_mag_index].magnitude) {
                highest_mag_index = i;
            }
	    }
	}

    if(highest_mag_index == -1) {
        return {0, 0};
    }

    return speed_report[highest_mag_index];
}

OPS243::range_report_t NearPassPredictor::get_range_of_highest_mag_m() {
    int highest_mag_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (range_reports[i].range_m != 0 && range_reports[i].range_m >= MINIMUM_RANGE_THRESHOLD) {
            if (range_reports[i].magnitude > RANGE_MAGNITUDE_THRESHOLD && range_reports[i].magnitude > range_reports[highest_mag_index].magnitude) {
                highest_mag_index = i;
            }
        }
    }

    if(highest_mag_index == -1) {
        return {0, 0};
    }
    
    return range_report[highest_mag_index];
}



