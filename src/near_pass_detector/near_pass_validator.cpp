
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

VehicleApproachDetector::VehicleApproachDetector(const char serial_port[], int BAUD_RATE) 
    : OPS243(port, baud_rate) {
    return;
}

VehicleApproachDetector::~VehicleApproachDetector() 
    : ~OPS243() {
        this->turn_range_reporting_off();
        this->turn_speed_reporting_off();
}

VehicleApproachDetector::set_flag_high() {
    flag = true;
}

void VehicleApproachDetector::initialize_sensor() {
    set_output_to_binary();
    set_inbound_only();
    set_maximum_range_filter(MAXIMUM_RANGE_THRESHOLD);
    set_minimum_speed_filter(MINIMUM_SPEED_THRESHOLD);
}

bool VehicleApproachDetector::is_vehicle_approaching (){
    bool vehicle_in_range = false;

    std::string serial_data = this->get_serial_data();
    if ((serial_data & SPEED_MASK) == EXPECTED_SPEED_MASK) {
            set_speed_value(serial_data && SPEED_VALUE_MASK);

            if (get_speed_value() > SPEED_THRESHOLD) {
                vehicle_in_range = is_vehicle_in_range(unsigned int serial_data);
                if (vehicle_in_range) {
                    raise_flag();
                }
            } else { return; }
            
    }
}

bool VehicleApproachDetector::is_vehicle_in_range(unsigned int serial_data) {
    bool is_vehicle_in_range;
    unsigned int vehicle_range;

    vehicle_range = (serial_data && EXPECTED_RANGE_MASK) >> 16;

    if (vehicle_range < MAXIMUM_RANGE_VALUE) {
        if (vehicle_range_magnitude > MAGNITUDE_THRESHOLD) {
            return true;
        }
    }
}

