
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

static const char LOW = 0;
static const char HIGH = 1;

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

static NearPassDetector* near_pass_detector = nullptr;

static void write_ride_object(Ride& ride) {
    write_ctic(localnode(), CTIC_R_ID,          (unsigned char*)(&ride.rideId),     sizeof(ride.rideId));
    write_ctic(localnode(), CTIC_R_START_TIME,  (unsigned char*)(&ride.startTime),  sizeof(ride.startTime));
    write_ctic(localnode(), CTIC_R_END_TIME,    (unsigned char*)(&ride.endTime),    sizeof(ride.endTime));
}

static void write_near_pass_object(NearPass& near_pass) {
    write_ctic(localnode(), CTIC_NP_TIME,           (unsigned char*)(&near_pass.time),          sizeof(near_pass.time));
    write_ctic(localnode(), CTIC_NP_DISTANCE_CM,    (unsigned char*)(&near_pass.distance_cm),   sizeof(near_pass.distance_cm));
    write_ctic(localnode(), CTIC_NP_SPEED_MPS,      (unsigned char*)(&near_pass.speed_mps),     sizeof(near_pass.speed_mps));
    write_ctic(localnode(), CTIC_NP_LATITUDE,       (unsigned char*)(&near_pass.latitude),      sizeof(near_pass.latitude));
    write_ctic(localnode(), CTIC_NP_LONGITUDE,      (unsigned char*)(&near_pass.longitude),     sizeof(near_pass.longitude));
    write_ctic(localnode(), CTIC_NP_RIDE_ID,        (unsigned char*)(&near_pass.rideId),        sizeof(near_pass.rideId));
}

static void le_client_write_callback(int ctic_index) {
    printf("le_server: Client wrote index %d\n", ctic_index);

    unsigned char read_buf[16];
    int num_bytes = read_ctic(localnode(), ctic_index, read_buf, sizeof(read_buf));
    if(num_bytes == 0) {
        printf("le_server: Client wrote 0 bytes to index %d\n", ctic_index);
    }

    // Next state logic
    server_state_t next_state = server_state;
    switch(server_state) {
        case SS_IDLE: {
            // If client wrote 1 to RL_REQUEST, initiate sending ride list
            // If client wrote 1 to NPL_REQUEST, initiate sending near pass list
            switch(ctic_index) {
                case CTIC_RL_REQUEST: {
                    int rl_request = *(int*)read_buf;
                    printf("rl_request: %d\n", rl_request);
                    if(rl_request) {
                        current_ride_list.clear();
                        current_ride_index = 0;
                        db_get_rides(current_ride_list);

                        if(current_ride_list.size() == 0) {
                            printf("rl_request: no rides to return\n");
                        }

                        // If at least 1 ride, write first ride and go to RL_REQUEST state
                        // If 0 or 1 ride, go back to idle
                        if(current_ride_list.size() > 0) {
                            write_ride_object(current_ride_list[current_ride_index]);
                            write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&HIGH), sizeof(HIGH));
                            current_ride_index++;
                            next_state = SS_RL_REQUEST;
                        }

                        if(current_ride_index >= current_ride_list.size()) {
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)(&LOW), sizeof(LOW));
                            next_state = SS_IDLE;
                        }
                    }
                    break;
                }
                case CTIC_NPL_REQUEST: {
                    int npl_request = *(int*)read_buf;
                    printf("npl_request: %d\n", npl_request);
                    if(npl_request) {
                        printf("le_server: Near pass list requested\n");
                        current_near_pass_list.clear();
                        current_near_pass_index = 0;
                        db_get_near_passes(current_near_pass_list);

                        // If at least 1 np, write first ride and go to NPL_REQUEST state
                        // If 0 or 1 np, go back to idle
                        if(current_near_pass_list.size() > 0) {
                            write_near_pass_object(current_near_pass_list[current_near_pass_index]);
                            write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&HIGH), sizeof(HIGH));
                            current_near_pass_index++;
                            next_state = SS_NPL_REQUEST;
                        }

                        if(current_near_pass_index >= current_near_pass_list.size()) {
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)(&LOW), sizeof(LOW));
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
                    int rl_request = *(int*)read_buf;
                    if(rl_request == 0) {
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&LOW), sizeof(LOW));
                        next_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_R_VALID: {
                    int r_valid = *(int*)read_buf;
                    if(r_valid == 0) {
                        write_ride_object(current_ride_list[current_ride_index]);
                        write_ctic(localnode(), CTIC_R_VALID, (unsigned char*)(&HIGH), sizeof(HIGH));
                        current_ride_index++;

                        if(current_ride_index >= current_ride_list.size()) {
                            write_ctic(localnode(), CTIC_RL_REQUEST, (unsigned char*)(&LOW), sizeof(LOW));
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
                    int npl_request = *(int*)read_buf;
                    if(npl_request == 0) {
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&LOW), sizeof(LOW));
                        next_state = SS_IDLE;
                    }
                    break;
                }
                case CTIC_NP_VALID: {
                    int np_valid = *(int*)read_buf;
                    if(np_valid == 0) {
                        write_near_pass_object(current_near_pass_list[current_near_pass_index]);
                        write_ctic(localnode(), CTIC_NP_VALID, (unsigned char*)(&HIGH), sizeof(HIGH));
                        current_near_pass_index++;

                        if(current_near_pass_index >= current_near_pass_list.size()) {
                            write_ctic(localnode(), CTIC_NPL_REQUEST, (unsigned char*)(&LOW), sizeof(LOW));
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
            double latitude = *(double*)read_buf;
            near_pass_detector->set_latitude(latitude);
            break;
        }
        case CTIC_GPS_LONGITUDE: {
            double longitude = *(double*)read_buf;
            near_pass_detector->set_longitude(longitude);
            break;
        }
        case CTIC_GPS_SPEED_MPS: {
            double speed_mps = *(double*)read_buf;
            near_pass_detector->set_speed_mps(speed_mps);
            break;
        }
        case CTIC_RC_CMD: {
            rc_cmd_t rc_cmd = *(rc_cmd_t*)read_buf;

            switch(rc_cmd) {
                case RC_CMD_START_RIDE:
                    near_pass_detector->start_ride();
                    break;

                case RC_CMD_END_RIDE:
                    near_pass_detector->end_ride();
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
        case LE_CONNECT: {
            printf("le_server: Client Connected\n");
            break;
        }
        case LE_READ:
            break;
        case LE_WRITE:
            le_client_write_callback(ctic_index);
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
    le_server(le_server_callback, 10);
    close_all();
}

void le_server_start(char* device_file) {
    if(init_blue(device_file) == 0) {
        printf("Couldn't init bluetooth\n");
        exit(1);
    }

    // The following 4 lines need to happen for some reason that we don't understand
    static unsigned char address[6] = {0xD3, 0x56, 0xDB, 0x24, 0xFF, 0xFF};
    set_le_random_address(address);
    set_le_wait(5000);
    le_pair(localnode(), JUST_WORKS, 0);

    const char DEVICE_NAME[] = "NPITS";
    write_ctic(localnode(), CTIC_DEVICE_NAME, (unsigned char*)DEVICE_NAME, sizeof(DEVICE_NAME));

    do_run_le_server = true;
    if(near_pass_detector == nullptr) {
        near_pass_detector = new NearPassDetector();
    }
    db_open();
    le_server_thread = new std::thread(run_le_server);
}

void le_server_stop() {
    do_run_le_server = false;
    if(le_server_thread->joinable()) {
        le_server_thread->join();
        delete le_server_thread;
    }
}