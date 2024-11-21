
#ifndef CTIC_INDEX_H
#define CTIC_INDEX_H

enum ctic_index_t {
    // Generic Access Service
    CTIC_DEVICE_NAME = 0,

    // Get Rides List Service
    CTIC_RL_REQUEST,
    CTIC_R_VALID,
    CTIC_R_ID,
    CTIC_R_START_UNIX_TIME,
    CTIC_R_END_UNIX_TIME,

    // Get Near Pass List Service
    CTIC_NPL_REQUEST,
    CTIC_NPL_ID,
    CTIC_NP_VALID,
    CTIC_NP_UNIX_TIME,
    CTIC_NP_DISTANCE_CM,
    CTIC_NP_SPEED_MPS,
    CTIC_NP_LATITUDE,
    CTIC_NP_LONGITUDE,

    // Give GPS Coords Service
    CTIC_GPS_LATIDUDE,
    CTIC_GPS_LONGITUDE,
    CTIC_GPS_SPEED_MPS
};

#endif