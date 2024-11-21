
#ifndef NEAR_PASS_DETECTOR_H
#define NEAR_PASS_DETECTOR_H

#include <string>

void near_pass_detector_init();

void near_pass_detector_start();
void near_pass_detector_stop();

void near_pass_detector_set_latitude(double latitude);
void near_pass_detector_set_longitude(double longitude);
void near_pass_detector_set_speed_mps(double speed_mps);

#endif