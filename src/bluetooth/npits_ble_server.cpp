
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <vector>

#include "../common/time_tools.h"
#include "../common/log.h"
#include "../connection_params.h"
#include "../db/near_pass_db.h"
#include "../near_pass_detection/near_pass_detection_types.h"
#include "../near_pass_detection/NearPassDetector.h"

#include "btlib.h"
#include "npits_ble_types.h"
#include "npits_ble_params.h"

#include "npits_ble_server.h"

#include "../devices/gopro.h"
#include "../devices/wifi.h"



static bool initialized = false;

static NearPassDetector* s_near_pass_detector = nullptr;
static NearPassPredictor* s_near_pass_predictor = nullptr;

static server_state_t server_state = SS_IDLE;
static long long request_start_time_ms = 0;

static std::vector<Ride> ride_list;
static std::vector<NearPass> near_pass_list;
static int ride_index = 0;
static int near_pass_index = 0;

static bool do_run_le_server = false;

void npits_ble_server_init(
    std::string devices_file,
    NearPassDetector* near_pass_detector,
    NearPassPredictor* near_pass_predictor
) {
    s_near_pass_detector = near_pass_detector;
    s_near_pass_predictor = near_pass_predictor;

    if(init_blue((char*)devices_file.c_str()) == 0) {
        log("LE Server", "Error: Couldn't init bluetooth");
        exit(1);
    }

    initialized = true;
}

static void write_ride_object(Ride& ride) {
    write_ctic(localnode(), CTIC_R_ID,          (unsigned char*)(&ride.rideId),     0);
    write_ctic(localnode(), CTIC_R_START_TIME,  (unsigned char*)(&ride.startTime),  0);
    write_ctic(localnode(), CTIC_R_END_TIME,    (unsigned char*)(&ride.endTime),    0);
}

static void write_near_pass_object(NearPass& near_pass) {
    write_ctic(localnode(), CTIC_NP_TIME,           (unsigned char*)(&near_pass.time),          0);
    write_ctic(localnode(), CTIC_NP_DISTANCE_CM,    (unsigned char*)(&near_pass.distance_cm),   0);
    write_ctic(localnode(), CTIC_NP_SPEED_MPS,      (unsigned char*)(&near_pass.speed_mps),     0);
    write_ctic(localnode(), CTIC_NP_LATITUDE,       (unsigned char*)(&near_pass.latitude),      0);
    write_ctic(localnode(), CTIC_NP_LONGITUDE,      (unsigned char*)(&near_pass.longitude),     0);
    write_ctic(localnode(), CTIC_NP_RIDE_ID,        (unsigned char*)(&near_pass.rideId),        0);
}

static void write_callback(int ctic_index) {
    unsigned char read_buf[16];
    int num_bytes = read_ctic(localnode(), ctic_index, read_buf, sizeof(read_buf));
    if(num_bytes == 0) {
        log("LE Server", "Client wrote 0 bytes");
    }

    // State change logic
    switch(server_state) {
        case SS_IDLE: {
            // If client wrote 1 to RL_REQUEST, initiate sending ride list
            // If client wrote 1 to NPL_REQUEST, initiate sending near pass list
            switch(ctic_index) {
                case CTIC_RL_REQUEST: {
                    int rl_request = *(int*)read_buf;
                    log("RL request", "Request flag: " + std::to_string(rl_request));
                    if(rl_request) {
                        request_start_time_ms = get_time_ms();
                        ride_index = 0;
                        db_get_rides(ride_list);

                        // If at least 1 object, write first object and go to RL_REQUEST state
                        // If 0 or 1 objects, go back to idle
                        if(ride_list.size() > 0) {
                            write_ride_object(ride_list[ride_index]);
                            write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&HIGH), 0);
                            log("RL request", "Wrote ride object " + std::to_string(ride_index));
                            ride_index++;
                            server_state = SS_RL_REQUEST;
                        }
                        else {
                            log("RL request", "No objects to return");
                        }

                        if(ride_index >= ride_list.size()) {
                            log("RL request", "Done");
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)(&LOW), 0);
                            server_state = SS_IDLE;
                        }
                    }
                    break;
                }
                case CTIC_NPL_REQUEST: {
                    int npl_request = *(int*)read_buf;
                    log("NPL request", "Request flag: " + std::to_string(npl_request));
                    if(npl_request) {
                        request_start_time_ms = get_time_ms();
                        near_pass_index = 0;
                        db_get_near_passes(near_pass_list);

                        // If at least 1 object, write first object and go to NPL_REQUEST state
                        // If 0 or 1 objects, go back to idle
                        if(near_pass_list.size() > 0) {
                            write_near_pass_object(near_pass_list[near_pass_index]);
                            write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&HIGH), 0);
                            log("NPL request", "Wrote near pass object " + std::to_string(near_pass_index));
                            near_pass_index++;
                            server_state = SS_NPL_REQUEST;
                        }
                        else {
                            log("NPL request", "No objects to return");
                        }

                        if(near_pass_index >= near_pass_list.size()) {
                            log("NPL request", "Done");
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)(&LOW), 0);
                            server_state = SS_IDLE;
                        }
                    }
                    break;
                }
                default:
                    break;
            } // switch ctic
            break;
        } // case SS_IDLE
        case SS_RL_REQUEST: {
            // If client wrote 0 to RL_REQUEST, cancel sending ride list
            // If client wrote 0 to R_VALID, send next ride
            switch(ctic_index) {
                case CTIC_RL_REQUEST: {
                    int rl_request = *(int*)read_buf;
                    if(rl_request == 0) {
                        log("RL Request", "Cancelled");
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&LOW), 0);
                        server_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_R_VALID: {
                    int r_valid = *(int*)read_buf;
                    if(r_valid == 0) {
                        write_ride_object(ride_list[ride_index]);
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&HIGH), 0);
                        log("RL request", "Wrote ride object " + std::to_string(ride_index));
                        ride_index++;

                        if(ride_index >= ride_list.size()) {
                            log("RL request", "Done");
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)(&LOW), 0);
                            server_state = SS_IDLE;
                        }
                    }
                    break;
                }
                default:
                    break;
            } // switch ctic
            break;
        } // case SS_RL_REQUEST
        case SS_NPL_REQUEST: {
            // If client wrote 0 to NPL_REQUEST, cancel sending near pass list
            // iF client wrote 0 to NP_VALID,  send next near pass
            switch(ctic_index) {
                case CTIC_NPL_REQUEST: {
                    int npl_request = *(int*)read_buf;
                    if(npl_request == 0) {
                        log("NPL request", "Cancelled");
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&LOW), 0);
                        server_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_NP_VALID: {
                    int np_valid = *(int*)read_buf;
                    if(np_valid == 0) {
                        write_near_pass_object(near_pass_list[near_pass_index]);
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&HIGH), 0);
                        log("NPL request", "Wrote near pass object " + std::to_string(near_pass_index));
                        near_pass_index++;

                        if(near_pass_index >= near_pass_list.size()) {
                            log("NPL request", "Done");
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)(&LOW), 0);
                            server_state = SS_IDLE;
                        }
                    }
                    break;
                }
                default:
                    break;
            } // switch ctic
            break;
        } // case SS_NPL_REQUEST
        default:
            break;
    } // Switch server_state

    // Non state machine ctics
    switch(ctic_index) {
        case CTIC_GPS_LATIDUDE: {
            if(s_near_pass_detector != nullptr) {
                double latitude = *(double*)read_buf;
                s_near_pass_detector->set_latitude(latitude);
                log("BLE Server", "Latitude: " + std::to_string(latitude));
            }
            break;
        }
        case CTIC_GPS_LONGITUDE: {
            if(s_near_pass_detector != nullptr) {
                double longitude = *(double*)read_buf;
                s_near_pass_detector->set_longitude(longitude);
                log("BLE Server", "Longitude: " + std::to_string(longitude));
            }
            break;
        }
        case CTIC_GPS_SPEED_MPS: {
            if(s_near_pass_detector != nullptr) {
                double speed_mps = *(double*)read_buf;
                s_near_pass_detector->set_cyclist_speed_mps(speed_mps);
                log("BLE Server", "Speed mps: " + std::to_string(speed_mps));
            }
            break;
        }
        case CTIC_GPS_TIME: {
            long time = *(long*)read_buf;
            set_time_s(time);
            log("BLE Server", "Time: " + std::to_string(time));
            break;
        }

        case CTIC_RC_CMD: {
            rc_cmd_t rc_cmd = *(rc_cmd_t*)read_buf;

            switch(rc_cmd) {
                case RC_CMD_START_RIDE:
                    log("LE Server", "Client started ride");

                    db_start_ride();

                    if(gopro_isConnected()) {
                        log("LE Server", "GoPro is connected");
                        gopro_start_recording();
                    }
                    else {
                        log("LE Server", "No GoPro connected");
                    }

                    if(s_near_pass_predictor != nullptr) {
                        s_near_pass_predictor->start();
                    }
                    else {
                        log("LE Server", "Warning: No predictor active");
                    }

                    if(s_near_pass_detector != nullptr) {
                        s_near_pass_detector->start();
                    }
                    else {
                        log("LE Server", "Warning: No detector active");
                    }

                    break;

                case RC_CMD_NONE:
                    log("LE Server", "Client ended ride");

                    if(s_near_pass_detector != nullptr) {
                        s_near_pass_detector->stop();
                    }
                    if(s_near_pass_predictor != nullptr) {
                        s_near_pass_predictor->stop();
                    }

                    db_end_ride();

                    gopro_stop_recording();

                    // gopro_post_process_ride();

                    break;

                default:
                    log("LE Server", "Unknown ride control command " + std::to_string((int)rc_cmd));
                    break;
            }
            break;
        }
        default:
            break;
    }
}

static void timer_callback() {
    switch(server_state) {
        case SS_IDLE:
            break;
        case SS_RL_REQUEST: {
            if(get_time_ms() - request_start_time_ms >= REQUEST_TIMEOUT_DURATION_ms) {
                log("RL request", "Timeout");
                server_state = SS_IDLE;
                write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)(&LOW), 0);
                write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&LOW), 0);
            }
            break;
        }
        case SS_NPL_REQUEST: {
            if(get_time_ms() - request_start_time_ms >= REQUEST_TIMEOUT_DURATION_ms) {
                log("NPL request", "Timeout");
                server_state = SS_IDLE;
                write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)(&LOW), 0);
                write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&LOW), 0);
            }
            break;
        }
        default:
            break;
    }

    db_update_current_ride_end_time();

    if(gopro_isConnected()) {
        write_ctic(localnode(), CTIC_GOPRO_STATUS, (unsigned char*)(&HIGH), 0);
    }
    else {
        write_ctic(localnode(), CTIC_GOPRO_STATUS, (unsigned char*)(&LOW), 0);
    }
}

static int server_callback(int client_node, int operation, int ctic_index) {
    switch(operation) {
        case LE_CONNECT: {
            log("LE Server", "Client Connected at node " + std::to_string(client_node));
            break;
        }
        case LE_READ:
            break;
        case LE_WRITE: {
            log("LE Server", "Client wrote ctic index " + std::to_string(ctic_index));
            write_callback(ctic_index);
            break;
        }
        case LE_DISCONNECT: {
            log("LE Server", "Client Disconnected from node " + std::to_string(client_node));
            break;
        }
        case LE_TIMER: {
            timer_callback();
            break;
        }
        case LE_KEYPRESS:
            break;
        default:
            break;
    }

    if(!do_run_le_server) {
        return SERVER_EXIT;
    }

    return SERVER_CONTINUE;
}

void npits_ble_server_run() {
    if(!initialized) {
        log("LE Server", "Warning: Couldn't run, not initialized");
        return;
    }

    // The following 4 lines need to happen for some reason that we don't understand
    static unsigned char address[6] = {0xD3, 0x56, 0xDB, 0x24, 0xFF, 0xFF};
    set_le_random_address(address);
    set_le_wait(5000);
    le_pair(localnode(), JUST_WORKS, 0);

    const char DEVICE_NAME[] = "NPITS";
    write_ctic(localnode(), CTIC_DEVICE_NAME, (unsigned char*)DEVICE_NAME, sizeof(DEVICE_NAME));

    do_run_le_server = true;

    const int SERVER_CALLBACK_PERIOD_ds = 10;

    // Blocking: thread is inside le_server() until 'x' is pressed on keyboard. Then the server stops
    // server_callback is called every SERVER_CALLBACK_PERIOD_ds deciseconds or on operation
    le_server(server_callback, SERVER_CALLBACK_PERIOD_ds);
    close_all();
    log("LE server", "Stopped");
}

void npits_ble_server_stop() {
    do_run_le_server = false;
}