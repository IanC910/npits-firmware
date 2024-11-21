
#ifndef NEAR_PASS_DETECTION_H
#define NEAR_PASS_DETECTION_H

#include <string>

void near_pass_detection_init();

void near_pass_detection_start();
void near_pass_detection_stop();

void near_pass_detection_set_latitude(double latitude);
void near_pass_detection_set_longitude(double longitude);
void near_pass_detection_set_speed_mps(double speed_mps);

#endif