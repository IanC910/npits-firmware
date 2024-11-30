
#include <stdio.h>
#include <chrono>
#include <string>
#include <thread>

#include "../common/time_tools.h"
#include "../devices/MB1242.h"
#include "../db/near_pass_db.h"

#include "near_pass_detection_params.h"
#include "near_pass_detection_types.h"
#include "NearPassDetector.h"



NearPassDetector::NearPassDetector(MB1242* ultrasonic) {
    this->ultrasonic = ultrasonic;

    if(ultrasonic == nullptr) {
        printf("NearPassDetector: Warning, ultrasonic is nullptr\n");
    }
}

NearPassDetector::~NearPassDetector() {
    stop();
}

int NearPassDetector::start() {
    if(ultrasonic == nullptr) {
        return 1;
    }

    if(is_active()) {
        return 1;
    }

    curr_ride_id = db_start_ride(get_time_s());

    do_run = true;
    detector_thread = new std::thread(&NearPassDetector::run, this);

    return 0;
}

int NearPassDetector::stop() {
    if(!is_active()) {
        return 1;
    }

    do_run = false;
    if(detector_thread->joinable()) {
        detector_thread->join();
        delete detector_thread;
        detector_thread = nullptr;
    }

    db_end_ride(curr_ride_id, get_time_s());

    return 0;
}

bool NearPassDetector::is_active() {
    return (detector_thread != nullptr);
}

void NearPassDetector::set_latitude(double latitude) {
    latest_latitude = latitude;
}

void NearPassDetector::set_longitude(double longitude) {
    latest_longitude = longitude;
}

void NearPassDetector::set_speed_mps(double speed_mps) {
    latest_speed_mps = speed_mps;
}

void NearPassDetector::run() {
    if(ultrasonic == nullptr) {
        printf("NearPassDetector: Couldn't run, ultrasonic is nullptr\n");
        return;
    }

    ultrasonic->begin_sampling();

    near_pass_state_t near_pass_state = NPS_NONE;

    long long near_pass_start_time_ms = get_time_ms();
    long long near_pass_end_time_ms = 0;
    int min_distance_cm = MB1242_MAX_DISTANCE_cm;

    while(do_run) {
        while(!ultrasonic->is_new_report_available()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        MB1242::report report = ultrasonic->get_latest_report();

        printf("%lld: %3d cm\n",
            report.time_stamp_ms,
            report.distance_cm
        );

        if(report.distance_cm < min_distance_cm) {
            min_distance_cm = report.distance_cm;
        }

        switch(near_pass_state) {
            case NPS_NONE: {
                // Condition to enter a near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm) {
                    printf("Near pass detector: Near pass potentially started\n");
                    near_pass_state = NPS_POTENTIALLY_STARTED;
                    near_pass_start_time_ms = report.time_stamp_ms;
                }
                break;
            }
            case NPS_POTENTIALLY_STARTED: {
                // Condition to confirm near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm) {
                    int current_duration_ms = report.time_stamp_ms - near_pass_start_time_ms;
                    if(current_duration_ms >= NEAR_PASS_STABILITY_DURATION_ms) {
                        printf("Near pass detector: In near pass\n");
                        near_pass_state = NPS_IN_NEAR_PASS;
                    }
                }
                // Else, cancel the near pass
                else {
                    printf("Near pass detector: Near pass entrance cancelled\n");
                    near_pass_state = NPS_NONE;
                }
                break;
            }
            case NPS_IN_NEAR_PASS: {
                // Condition to exit a near pass
                if(report.distance_cm > DISTANCE_THRESHOLD_cm) {
                    printf("Near pass detector: Near pass potentially over\n");
                    near_pass_state = NPS_POTENTIALLY_OVER;
                    near_pass_end_time_ms = report.time_stamp_ms;
                }
                break;
            }
            case NPS_POTENTIALLY_OVER: {
                // Condition to confirm near pass is over
                if(report.distance_cm > DISTANCE_THRESHOLD_cm) {
                    int current_duration_ms = report.time_stamp_ms - near_pass_end_time_ms;
                    if(current_duration_ms > NEAR_PASS_STABILITY_DURATION_ms) {
                        printf("Near pass detector: Near pass over\n");
                        near_pass_state = NPS_NONE;

                        int near_pass_duration_ms = near_pass_end_time_ms - near_pass_start_time_ms;

                        // If near pass valid, log in db
                        if(near_pass_duration_ms >= NEAR_PASS_MIN_DURATION_ms &&
                            near_pass_duration_ms <= NEAR_PASS_MAX_DURATION_ms
                        ) {
                            printf("Near pass detector: Near pass valid, logging\n");
                            NearPass near_pass;
                            near_pass.time          = (long)(near_pass_start_time_ms / 1000);
                            near_pass.distance_cm   = min_distance_cm;
                            near_pass.speed_mps     = latest_speed_mps;
                            near_pass.latitude      = latest_latitude;
                            near_pass.longitude     = latest_longitude;

                            db_insert_near_pass(near_pass);
                        }

                        // Reset necessary params
                        min_distance_cm = MB1242_MAX_DISTANCE_cm;
                    }
                }
                // Else, stay in the near pass
                else {
                    printf("Near pass detector: Near pass exit cancelled\n");
                    near_pass_state = NPS_IN_NEAR_PASS;
                }
                break;
            }
            default:
                break;
        } // switch(state)
    } // while(do_run)

    ultrasonic->stop_sampling();
}