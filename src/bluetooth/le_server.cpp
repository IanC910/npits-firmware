
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

#include "../near_pass_tracking/near_pass_detection.h"

#include "btlib.h"
#include "le_types.h"

#include "le_server.h"

static bool do_run_le_server = false;
static std::thread le_server_thread;

static int le_client_write_callback(int client_node, int ctic_index) {
    switch(ctic_index) {
        case CTIC_RL_REQUEST:
            break;
        case CTIC_R_VALID:
            break;
        case CTIC_NPL_REQUEST:
            break;
        case CTIC_NPL_ID:
            break;
        case CTIC_NP_VALID:
            break;

        case CTIC_GPS_LATIDUDE:
            unsigned char read_buf[32];
            int num_bytes = read_ctic(client_node, ctic_index, read_buf, sizeof(read_buf));
            double latitude;
            sscanf((const char*)read_buf, "%lf", &latitude);
            near_pass_detection_set_latitude(latitude);
            break;

        case CTIC_GPS_LONGITUDE:
            unsigned char read_buf[32];
            int num_bytes = read_ctic(client_node, ctic_index, read_buf, sizeof(read_buf));
            double longitude;
            sscanf((const char*)read_buf, "%lf", &longitude);
            near_pass_detection_set_longitude(longitude);
            break;

        case CTIC_GPS_SPEED_MPS:
            unsigned char read_buf[32];
            int num_bytes = read_ctic(client_node, ctic_index, read_buf, sizeof(read_buf));
            double speed_mps;
            sscanf((const char*)read_buf, "%lf", &speed_mps);
            near_pass_detection_set_speed_mps(speed_mps);
            break;

        case CTIC_RC_CMD:
            break;

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
    le_server_thread = std::thread(run_le_server);
}

void le_server_stop() {
    do_run_le_server = false;
    if(le_server_thread.joinable()) {
        le_server_thread.join();
    }
}