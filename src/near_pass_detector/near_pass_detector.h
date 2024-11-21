
#ifndef NEAR_PASS_DETECTOR_H
#define NEAR_PASS_DETECTOR_H

#include <string>

// Returns 0 on success, 1 if already initialized
int near_pass_detector_init();

// Returns 0 on success, 1 if ride already active
int near_pass_detector_start_ride();

// Returns 0 on success, 1 if no ride is active
int near_pass_detector_end_ride();

// Returns true if a ride is currently active, false otherwise
bool near_pass_detector_is_active();

void near_pass_detector_set_latitude(double latitude);
void near_pass_detector_set_longitude(double longitude);
void near_pass_detector_set_speed_mps(double speed_mps);

#endif