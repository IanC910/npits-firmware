
#include <stdio.h>
#include <stdlib.h>

#include <string>
#include <thread>
#include <vector>

#include "../db/near_pass_db.h"
#include "../near_pass_detector/near_pass_detector_types.h"
#include "../near_pass_detector/near_pass_detector.h"

#include "btlib.h"
#include "le_types.h"

#include "le_server.h"

enum server_state_t {
    SS_IDLE,
    SS_RL_REQUEST,
    SS_NPL_REQUEST
};

static server_state_t server_state = SS_IDLE;
static std::vector<Ride> current_ride_list;
static std::vector<NearPass> current_near_pass_list;
static int current_ride_index = 0;
static int current_near_pass_index = 0;

static bool do_run_le_server = false;
static std::thread* le_server_thread;

static NearPassDetector near_pass_detector;

static void write_ride_object(Ride& ride) {
    std::string ride_id_str     = std::to_string(ride.rideId);
    std::string start_time_str  = std::to_string(ride.startTime);
    std::string end_time_str    = std::to_string(ride.endTime);

    write_ctic(localnode(), CTIC_R_ID,          (unsigned char*)ride_id_str.c_str(),    ride_id_str.length());
    write_ctic(localnode(), CTIC_R_START_TIME,  (unsigned char*)start_time_str.c_str(), start_time_str.length());
    write_ctic(localnode(), CTIC_R_END_TIME,    (unsigned char*)end_time_str.c_str(),   end_time_str.length());
}

static void write_near_pass_object(NearPass& near_pass) {
    std::string time_str        = std::to_string(near_pass.time);
    std::string distance_str    = std::to_string(near_pass.distance_cm);
    std::string speed_str       = std::to_string(near_pass.speed_mps);
    std::string latitude_str    = std::to_string(near_pass.latitude);
    std::string longitude_str   = std::to_string(near_pass.longitude);
    std::string ride_id_str     = std::to_string(near_pass.rideId);

    write_ctic(localnode(), CTIC_NP_TIME,           (unsigned char*)time_str.c_str(),       time_str.length());
    write_ctic(localnode(), CTIC_NP_DISTANCE_CM,    (unsigned char*)distance_str.c_str(),   distance_str.length());
    write_ctic(localnode(), CTIC_NP_SPEED_MPS,      (unsigned char*)speed_str.c_str(),      speed_str.length());
    write_ctic(localnode(), CTIC_NP_LATITUDE,       (unsigned char*)latitude_str.c_str(),   latitude_str.length());
    write_ctic(localnode(), CTIC_NP_LONGITUDE,      (unsigned char*)longitude_str.c_str(),  longitude_str.length());
    write_ctic(localnode(), CTIC_NPL_ID,            (unsigned char*)ride_id_str.c_str(),    ride_id_str.length());
}

static void le_client_write_callback(int client_node, int ctic_index) {
    unsigned char read_buf[32];
    int num_bytes = read_ctic(client_node, ctic_index, read_buf, sizeof(read_buf));

    // Next state logic
    server_state_t next_state = server_state;
    switch(server_state) {
        case SS_IDLE: {
            // If client wrote 1 to RL_REQUEST, initiate sending ride list
            // If client wrote 1 to NPL_REQUEST, initiate sending near pass list
            switch(ctic_index) {
                case CTIC_RL_REQUEST: {
                    int rl_request = atoi((const char*)read_buf);
                    if(rl_request) {
                        current_ride_list.clear();
                        current_ride_index = 0;
                        db_get_rides(current_ride_list);

                        // If at least 1 ride, write first ride and go to RL_REQUEST state
                        // If 0 or 1 ride, go back to idle
                        if(current_ride_list.size() > 0) {
                            write_ride_object(current_ride_list[current_ride_index]);
                            write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)"1", 1);
                            current_ride_index++;
                            next_state = SS_RL_REQUEST;
                        }

                        if(current_ride_index >= current_ride_list.size()) {
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)"0", 1);
                            next_state = SS_IDLE;
                        }
                    }
                    break;
                }
                case CTIC_NPL_REQUEST: {
                    int npl_request = atoi((const char*)read_buf);
                    if(npl_request) {
                        current_near_pass_list.clear();
                        current_near_pass_index = 0;
                        db_get_near_passes(current_near_pass_list);

                        // If at least 1 np, write first ride and go to NPL_REQUEST state
                        // If 0 or 1 np, go back to idle
                        if(current_near_pass_list.size() > 0) {
                            write_near_pass_object(current_near_pass_list[current_near_pass_index]);
                            write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)"1", 1);
                            current_near_pass_index++;
                            next_state = SS_NPL_REQUEST;
                        }

                        if(current_near_pass_index >= current_near_pass_list.size()) {
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)"0", 1);
                            next_state = SS_IDLE;
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
                    int rl_request = atoi((const char*)read_buf);
                    if(rl_request == 0) {
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)"0", 1);
                        next_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_R_VALID: {
                    int r_valid = atoi((const char*)read_buf);
                    if(r_valid == 0) {
                        write_ride_object(current_ride_list[current_ride_index]);
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)"1", 1);
                        current_ride_index++;

                        if(current_ride_index >= current_ride_list.size()) {
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)"0", 1);
                            next_state = SS_IDLE;
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
                    int npl_request = atoi((const char*)read_buf);
                    if(npl_request == 0) {
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)"0", 1);
                        next_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_NP_VALID: {
                    int np_valid = atoi((const char*)read_buf);
                    if(np_valid == 0) {
                        write_near_pass_object(current_near_pass_list[current_near_pass_index]);
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)"1", 1);
                        current_near_pass_index++;

                        if(current_near_pass_index >= current_near_pass_list.size()) {
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)"0", 1);
                            next_state = SS_IDLE;
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

    server_state = next_state;

    // Non state machine ctics
    switch(ctic_index) {
        case CTIC_GPS_LATIDUDE: {
            double latitude = strtod((const char*)read_buf, nullptr);
            near_pass_detector.set_latitude(latitude);
            break;
        }
        case CTIC_GPS_LONGITUDE: {
            double longitude = strtod((const char*)read_buf, nullptr);
            near_pass_detector.set_longitude(longitude);
            break;
        }
        case CTIC_GPS_SPEED_MPS: {
            double speed_mps = strtod((const char*)read_buf, nullptr);
            near_pass_detector.set_speed_mps(speed_mps);
            break;
        }
        case CTIC_RC_CMD: {
            rc_cmd_t rc_cmd = (rc_cmd_t)atoi((const char*)read_buf);

            switch(rc_cmd) {
                case RC_CMD_START_RIDE:
                    near_pass_detector.start_ride();
                    break;

                case RC_CMD_END_RIDE:
                    near_pass_detector.end_ride();
                    break;

                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
}

static int le_server_callback(int client_node, int operation, int ctic_index) {
    switch(operation) {
        case LE_CONNECT:
            printf("le_server: Client Connected\n");
            break;
        case LE_READ:
            break;
        case LE_WRITE:
            le_client_write_callback(client_node, ctic_index);
            break;
        case LE_DISCONNECT:
            printf("le_server: Client Disconnected\n");
            break;
        case LE_TIMER:
            break;
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

static void run_le_server() {
    // Write Info characteristics
    const char DEVICE_NAME[] = "NPITS";
    write_ctic(localnode(), CTIC_DEVICE_NAME, (unsigned char*)DEVICE_NAME, sizeof(DEVICE_NAME));

    le_server(le_server_callback, 10);
    close_all();
}

void le_server_start() {
    do_run_le_server = true;
    le_server_thread = new std::thread(run_le_server);
}

void le_server_stop() {
    do_run_le_server = false;
    if(le_server_thread->joinable()) {
        le_server_thread->join();
        delete le_server_thread;
    }
}