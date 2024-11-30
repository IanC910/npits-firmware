
#ifndef LE_PARAMS_H
#define LE_PARAMS_H

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

    // Give World Info Service
    CTIC_WI_LATIDUDE,
    CTIC_WI_LONGITUDE,
    CTIC_WI_SPEED_MPS,
    CTIC_WI_TIME,

    // Ride Control Service
    CTIC_RC_CMD
};

enum rc_cmd_t {
    RC_CMD_NONE,
    RC_CMD_START_RIDE
};

#endif