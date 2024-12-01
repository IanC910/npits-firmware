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

    log("NearPassPredictr", "Starting...");

    initialize_radar();

    do_run = true;
    while(do_run) {
        update_speeds_and_ranges();

        if(is_vehicle_approaching()) {
            log("NearPassPredictor", "Vehicle approaching");
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
        speed_magnitude_array,
        range_magnitude_array,
        speed_mps_array,
        range_m_array
    );
}

bool NearPassPredictor::is_vehicle_approaching() {
    int fastest_signal_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (speed_mps_array[i] != 0 && speed_mps_array[i] >= MINIMUM_SPEED_THRESHOLD && speed_mps_array[i] > fastest_signal_index)
        {
            speed_mps_array[i] = 0;
            if (speed_magnitude_array[i] > SPEED_MAGNITUDE_THRESHOLD) {
                fastest_signal_index = i;
            }
	    }
	}

    return fastest_signal_index != -1;
}

bool NearPassPredictor::is_vehicle_in_range() {
    int nearest_signal_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (range_m_array[i] != 0 && range_m_array[i] >= MINIMUM_RANGE_THRESHOLD && range_m_array[i] > nearest_signal_index) {
            if (range_magnitude_array[i] > RANGE_MAGNITUDE_THRESHOLD) {
                nearest_signal_index = i;
            }
        }
    }

    return nearest_signal_index != -1;
}

speed_report_t get_speed_of_approaching_vehicle_mps() {
    int fastest_signal_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (speed_mps_array[i] != 0 && speed_mps_array[i] >= MINIMUM_SPEED_THRESHOLD && speed_mps_array[i] > fastest_signal_index)
        {
            speed_mps_array[i] = 0;
            if (speed_magnitude_array[i] > SPEED_MAGNITUDE_THRESHOLD) {
                fastest_signal_index = i;
            }
	    }
	}

    if(fastest_signal_index == -1) {
        return {0, 0}
    }

    speed_report_t speed_report;
    speed_report.speed_mps = speed_mps_array[fastest_signal_index];
    speed_report.magnitude = speed_magnitude_array[fastest_signal_index];
    return speed_report;
}

range_report_t get_distance_of_highest_mag_m() {
    int nearest_signal_index = -1;

    for (int i = 0; i < OPS243::MAX_REPORTS; i++) {
        if (range_m_array[i] != 0 && range_m_array[i] >= MINIMUM_RANGE_THRESHOLD && range_m_array[i] > nearest_signal_index) {
            if (range_magnitude_array[i] > RANGE_MAGNITUDE_THRESHOLD) {
                nearest_signal_index = i;
            }
        }
    }

    if(nearest_signal_index == -1) {
        return {0, 0}
    }

    range_report_t range_report;
    range_report.range_m = range_m_array[nearest_signal_index];
    range_repor.magnitude = range_magnitude_array[nearest_signal_index];
    return range_report;
}



