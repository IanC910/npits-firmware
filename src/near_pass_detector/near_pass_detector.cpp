
#include <chrono>
#include <string>
#include <thread>

#include "../common/time_tools.h"
#include "../devices/MB1242.h"
#include "../db/near_pass_db.h"
#include "../connection_params.h"

#include "near_pass_detector_params.h"
#include "near_pass_detector_types.h"
#include "near_pass_detector.h"

void NearPassDetector::run_near_pass_detector() {
    ultrasonic.begin_sampling();

    near_pass_state_t near_pass_state = NPS_NONE;

    long long last_near_pass_time_ms = get_time_ms();
    long long near_pass_start_time_ms = get_time_ms();
    int min_distance_cm = MB1242_MAX_DISTANCE_cm;
    int near_pass_duration_ms = 0;

    while(do_run_near_pass_detector) {
        while(!ultrasonic.is_new_report_available()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        MB1242::report report = ultrasonic.get_latest_report();

        if(report.distance_cm < min_distance_cm) {
            min_distance_cm = report.distance_cm;
        }

        switch(near_pass_state) {
            case false:
                // Condition to enter a near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm &&
                    report.time_stamp_ms > last_near_pass_time_ms + NEAR_PASS_COOLDOWN_ms
                ) {
                    printf("Near pass detector: Start of near pass\n");
                    near_pass_state = NPS_IN_NEAR_PASS;
                    last_near_pass_time_ms = report.time_stamp_ms;
                    near_pass_start_time_ms = report.time_stamp_ms;

                    // TODO: validate and clip gopro footage
                }
                break;
            case true:
                // Condition to exit a near pass
                if(report.distance_cm > DISTANCE_THRESHOLD_cm) {
                    printf("Near pass detector: End of near pass\n");
                    near_pass_state = NPS_NONE;
                    near_pass_duration_ms = report.time_stamp_ms - near_pass_start_time_ms;

                    // If the near pass is valid
                    // TODO (Maybe): get info from extra validator thread
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

                        // TODO (Maybe): Send to validator instead of logging in db
                        db_insert_near_pass(near_pass);
                    }

                    // Reset necessary params
                    min_distance_cm = MB1242_MAX_DISTANCE_cm;
                }
                break;
            default:
                break;
        } // switch
    }

    ultrasonic.stop_sampling();
}

NearPassDetector::NearPassDetector() :
    ultrasonic(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM)
{}

int NearPassDetector::start_ride() {
    if(is_ride_active) {
        return 1;
    }

    is_ride_active = true;
    curr_ride_id = db_start_ride(get_time_s());

    do_run_near_pass_detector = true;
    detector_thread = new std::thread(&NearPassDetector::run_near_pass_detector, this);

    return 0;
}

int NearPassDetector::end_ride() {
    if(!is_ride_active) {
        return 1;
    }

    do_run_near_pass_detector = false;
    if(detector_thread->joinable()) {
        detector_thread->join();
        delete detector_thread;
    }

    db_end_ride(curr_ride_id, get_time_s());

    is_ride_active = false;

    return 0;
}

bool NearPassDetector::get_is_ride_active() {
    return is_ride_active;
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