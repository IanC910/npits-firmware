
#include <chrono>
#include <string>
#include <thread>

#include "../common/time_tools.h"
#include "../common/structs.h"
#include "../devices/MB1242.h"
#include "../db/near_pass_db.h"
#include "../connection_params.h"

#include "near_pass_detection_params.h"
#include "near_pass_detection.h"

static bool initialized = false;

static MB1242 ultrasonic;

static bool do_run_detector = false;
static std::thread detector_thread;

static double latest_latitude = 0;
static double latest_longitude = 0;
static double latest_speed_mps = 0;

static void run_detector() {
    ultrasonic.begin_sampling();

    bool in_near_pass = false;
    long long last_near_pass_time_ms = get_time_ms();
    long long near_pass_start_time_ms = get_time_ms();
    int min_distance_cm = MB1242_MAX_DISTANCE_cm;
    int near_pass_duration_ms = 0;

    while(do_run_detector) {
        while(!ultrasonic.is_new_report_available()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        MB1242::report report = ultrasonic.get_latest_report();

        switch(in_near_pass) {
            case false:
                // Condition to enter a near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm &&
                    report.time_stamp_ms > last_near_pass_time_ms + NEAR_PASS_COOLDOWN_ms
                ) {
                    in_near_pass = true;
                    last_near_pass_time_ms = report.time_stamp_ms;
                    near_pass_start_time_ms = report.time_stamp_ms;

                    // TODO: validate and clip gopro footage
                }
                // No break
            case true:
                // Condition to remain in a near pass
                if(report.distance_cm <= DISTANCE_THRESHOLD_cm) {
                    if(report.distance_cm < min_distance_cm) {
                        min_distance_cm = report.distance_cm;
                    }
                }
                // Condition to exit a near pass
                else {
                    in_near_pass = false;

                    near_pass_duration_ms = report.time_stamp_ms - near_pass_start_time_ms;

                    // If the near pass is valid
                    // TODO (Maybe): get info from extra validator thread
                    if(near_pass_duration_ms >= NEAR_PASS_MIN_DURATION_ms &&
                        near_pass_duration_ms <= NEAR_PASS_MAX_DURATION_ms
                    ) {
                        NearPass near_pass;
                        near_pass.time          = (long)(near_pass_start_time / 1000);
                        near_pass.distance_cm   = min_distance_cm;
                        near_pass.speed         = latest_speed_mps;
                        near_pass.latitude      = latest_latitude;
                        near_pass.longitude     = latest_longitude;

                        // TODO (Maybe): Send to validator instead of logging in db
                        db_insert_near_pass(db, near_pass);
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

void near_pass_detection_init() {
    if(initialized) {
        return;
    }

    ultrasonic = MB1242(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);

    initialized = true;
}

void near_pass_detection_start() {
    near_pass_detection_init()
    detector_thread = std::thread(run_detector);
}

void near_pass_detection_stop() {
    do_run_detector = false;
    if(detector_thread.joinable()) {
        detector_thread.join();
    }
}

void near_pass_detection_set_latitude(double latitude) {
    latest_latitude = latitude;
}

void near_pass_detection_set_longitude(double longitude) {
    lastest_longitude = longitude;
}

void near_pass_detection_set_speed_mps(double speed_mps) {
    latest_speed_mps = speed_mps;
}