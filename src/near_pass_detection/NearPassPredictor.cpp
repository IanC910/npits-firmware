#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "../devices/OPS243.h"

#include "near_pass_prediction_params.h"
#include "NearPassPredictor.h"

NearPassPredictor::NearPassPredictor(OPS243* radar) {
    this->radar = radar;

    if(radar == nullptr) {
        printf("NearPassPredictor: Warning, radar is nullptr\n");
    }
}

NearPassPredictor::~NearPassPredictor() {
    stop();
}

int NearPassPredictor::start() {
    if(radar == nullptr) {
        return 1;
    }

    if(is_active()) {
        return 1;
    }

    initialize_radar();

    do_run = true;
    predictor_thread = new std::thread(&NearPassPredictor::run, this);

    return 0;
}

int NearPassPredictor::stop() {
    if(!is_active()) {
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

bool NearPassPredictor::is_active() {
    return (predictor_thread != nullptr);
}

void NearPassPredictor::initialize_radar() {
    if(radar == nullptr) {
        printf("NearPassPredictor: Coudn't initialize radar, radar is nullptr\n");
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
        printf("NearPassPredictor: Coudn't update, radar is nullptr\n");
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

    return fastest_signal_index;
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

    return nearest_signal_index;
}

void NearPassPredictor::run() {
    if(radar == nullptr) {
        printf("NearPassPredictor: Couldn't run, radar is nullptr\n");
        return;
    }

    initialize_radar();

    while(do_run) {
        update_speeds_and_ranges();

        if(is_vehicle_approaching() && is_vehicle_in_range()) {
            printf("NearPassPredictor: Prediction!\n");
        }
    }

    radar->turn_range_reporting_off();
    radar->turn_speed_reporting_off();
}



