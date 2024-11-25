#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include "near_pass_validator.h"
#include "OPS243.cpp"

near_pass_validator::near_pass_validator(const char serial_port[], int BAUD_RATE) 
    : OPS243(serial_port, BAUD_RATE) {
    // Constructor initialization using OPS243
}

near_pass_validator::~near_pass_validator() {
    // Destructor, clean up, calling base class destructor is automatic
    this->turn_range_reporting_off();
    this->turn_speed_reporting_off();
}

void near_pass_validator::set_flag_high() {
    flag = true;
}

void near_pass_validator::initialize_sensor() {
    set_number_of_range_reports(9);
    set_number_of_speed_reports(9);
    turn_units_output_on();
    turn_fmcw_magnitude_reporting_on();
    turn_doppler_magnitude_reporting_on();
    set_inbound_only();
    set_maximum_range_filter(MAXIMUM_RANGE_VALUE);
    set_minimum_speed_filter(MINIMUM_SPEED_THRESHOLD);
    enable_peak_speed_average();
    turn_range_reporting_on();
    turn_speed_reporting_on();
}

bool near_pass_validator::is_vehicle_approaching(float* speed_matrix, int* magnitude_matrix) {
    int fastest_signal = 0;
    bool ret = false;

    for (int i = 0; i < 9; i++) {
        if (speed_matrix[i] != NULL && speed_matrix[i] >= MINIMUM_SPEED_THRESHOLD && speed_matrix[i] > fastest_signal)
            if (magnitude_matrix[i] > SPEED_MAGNITUDE_THRESHOLD) {
                fastest_signal = i;   
                ret = true;
            }
    }

    return ret;
}

bool near_pass_validator::is_vehicle_in_range(float* range_matrix, int* magnitude_matrix) {
    int nearest_signal = 0;
    bool ret = true;

    for (int i = 0; i < 9; i++) {
        if (range_matrix[i] != NULL && range_matrix[i] >= MINIMUM_RANGE_THRESHOLD && range_matrix[i] > nearest_signal)
            if (magnitude_matrix[i] > RANGE_MAGNITUDE_THRESHOLD) {
                nearest_signal = i;
                ret = true;
		printf("Made it here\n");
            }
    }
/*
     	    printf("Magnitudes: ");
            for(int i = 0; i < sizeof(magnitude_matrix)/sizeof(magnitude_matrix[0]); i++) {
                printf("%d,", magnitude_matrix[i]);
            }

            printf("Ranges:");
            for(int j = 0; j < sizeof(range_matrix)/sizeof(range_matrix[0]); j++) {
                printf("%f,", range_matrix[j]);
            }
	    printf("\n");
*/
    return ret;
}

