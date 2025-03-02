
#include <stdio.h>
#include <chrono>
#include <string>
#include <thread>

#include "../common/time_tools.h"
#include "../common/log.h"
#include "../devices/MB1242.h"
#include "../db/near_pass_db.h"

#include "near_pass_detection_params.h"
#include "near_pass_detection_types.h"
#include "NearPassDetector.h"

#include "../devices/gopro.h"
#include "../devices/wifi.h"



NearPassDetector::NearPassDetector(MB1242* ultrasonic, bool use_predictor) {
    this->ultrasonic = ultrasonic;
    this->use_predictor = use_predictor;

    if(ultrasonic == nullptr) {
        log("NearPassDetector", "Warning, ultrasonic is nullptr");
    }
}

NearPassDetector::~NearPassDetector() {
    if(detector_thread != nullptr) {
        stop();
    }
}

int NearPassDetector::start() {
    if(ultrasonic == nullptr) {
        return 1;
    }

    if(detector_thread != nullptr) {
        return 1;
    }

    detector_thread = new std::thread(&NearPassDetector::run, this);

    return 0;
}

int NearPassDetector::stop() {
    if(detector_thread == nullptr) {
        log("NearPassDetector", "Couldn't stop, wasn't running");
        return 1;
    }

    do_run = false;
    if(detector_thread->joinable()) {
        detector_thread->join();
        delete detector_thread;
        detector_thread = nullptr;
        log("NearPassDetector", "Stopped");
    }

    return 0;
}

void NearPassDetector::run() {
    if(ultrasonic == nullptr) {
        log("NearPassDetector", "Couldn't run, ultrasonic is nullptr");
        return;
    }

    if(do_run) {
        log("NearPassDetector", "Couldn't run, already running");
        return;
    }

    log("NearPassDetector", "Starting...");

    ultrasonic->begin_sampling();

    near_pass_state_t near_pass_state = NPS_NONE;

    long long near_pass_start_time_ms = get_time_ms();
    long long near_pass_end_time_ms = 0;
    int min_distance_cm = MB1242::MAX_DISTANCE_cm;
    bool was_near_pass_predicted_at_start_time = false;

    do_run = true;
    while(do_run) {
        while(!ultrasonic->is_new_report_available()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        MB1242::report report = ultrasonic->get_latest_report();

        if(report.distance_cm < min_distance_cm) {
            min_distance_cm = report.distance_cm;
        }

        switch(near_pass_state) {
            case NPS_NONE: {
                // Condition to enter a near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm) {
                    log("NearPassDetector", "Near pass potentially started");
                    near_pass_state = NPS_POTENTIALLY_STARTED;
                    near_pass_start_time_ms = report.time_stamp_ms;
                    was_near_pass_predicted_at_start_time = is_near_pass_predicted_now;
                }
                break;
            }
            case NPS_POTENTIALLY_STARTED: {
                // Condition to confirm near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm) {
                    int current_duration_ms = report.time_stamp_ms - near_pass_start_time_ms;
                    if(current_duration_ms >= NEAR_PASS_STABILITY_DURATION_ms) {
                        log("NearPassDetector", "In near pass");
                        near_pass_state = NPS_IN_NEAR_PASS;
                    }
                }
                // Else, cancel the near pass
                else {
                    log("NearPassDetector", "Near pass entrance cancelled");
                    near_pass_state = NPS_NONE;
                }
                break;
            }
            case NPS_IN_NEAR_PASS: {
                // Condition to exit a near pass
                if(report.distance_cm > DISTANCE_THRESHOLD_cm) {
                    log("NearPassDetector", "Near pass potentially over");
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
                        log("NearPassDetector", "Near pass over");
                        near_pass_state = NPS_NONE;

                        int near_pass_duration_ms = near_pass_end_time_ms - near_pass_start_time_ms;

                        // If near pass valid, log in db
                        if(near_pass_duration_ms >= NEAR_PASS_MIN_DURATION_ms &&
                            near_pass_duration_ms <= NEAR_PASS_MAX_DURATION_ms
                        ) {
                            log("NearPassDetector", "Near pass valid");

                            if((use_predictor && was_near_pass_predicted_at_start_time) || !use_predictor) {
                                log("NearPassDetector", "Logging near pass");
                                NearPass near_pass;
                                near_pass.time          = (long)(near_pass_start_time_ms / 1000);
                                near_pass.distance_cm   = min_distance_cm;
                                near_pass.speed_mps     = latest_cyclist_speed_mps + latest_vehicle_speed_mps;
                                near_pass.latitude      = latest_latitude;
                                near_pass.longitude     = latest_longitude;
                                near_pass.rideId        = db_get_current_ride_id();

                                db_insert_near_pass(near_pass);
                                gopro_add_hilight_tag();
                            }
                            else if(use_predictor) {
                                log("NearPassDetector", "Not logging near pass, near pass wasn't predicted");
                            }
                        }

                        // Reset necessary params
                        min_distance_cm = MB1242::MAX_DISTANCE_cm;
                    }
                }
                // Else, stay in the near pass
                else {
                    log("NearPassDetector", "Near pass exit cancelled");
                    near_pass_state = NPS_IN_NEAR_PASS;
                }
                break;
            }
            default:
                break;
        } // switch(state)
    } // while(do_run)

    log("NearPassDetector", "Stopping...");

    ultrasonic->stop_sampling();
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

void NearPassDetector::set_cyclist_speed_mps(double cyclist_speed_mps) {
    latest_cyclist_speed_mps = cyclist_speed_mps;
}

void NearPassDetector::set_vehicle_speed_mps(double vehicle_speed_mps) {
    latest_vehicle_speed_mps = vehicle_speed_mps;
}

void NearPassDetector::set_prediction_flag(bool prediction) {
    is_near_pass_predicted_now = prediction;
}