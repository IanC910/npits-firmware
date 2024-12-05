
#ifndef NPITS_BLE_TYPES_H
#define NPITS_BLE_TYPES_H

enum ctic_t {
    // Generic Access Service
    CTIC_DEVICE_NAME,

    // Get Rides List Service
    CTIC_RL_REQUEST,
    CTIC_R_VALID,
    CTIC_R_ID,
    CTIC_R_START_TIME,
    CTIC_R_END_TIME,

    // Get Near Pass List Service
    CTIC_NPL_REQUEST,
    CTIC_NP_VALID,
    CTIC_NP_TIME,
    CTIC_NP_DISTANCE_CM,
    CTIC_NP_SPEED_MPS,
    CTIC_NP_LATITUDE,
    CTIC_NP_LONGITUDE,
    CTIC_NP_RIDE_ID,

    // Give GPS Info service
    CTIC_GPS_LATIDUDE,
    CTIC_GPS_LONGITUDE,
    CTIC_GPS_SPEED_MPS,
    CTIC_GPS_TIME,

    // Ride Control Service
    CTIC_RC_CMD,

    // GoPro Status Service
    CTIC_GOPRO_STATUS
};

enum rc_cmd_t {
    RC_CMD_NONE,
    RC_CMD_START_RIDE
};

enum server_state_t {
    SS_IDLE,
    SS_RL_REQUEST,
    SS_NPL_REQUEST
};

#endif