
#ifndef NEAR_PASS_DETECTION_TYPES_H
#define NEAR_PASS_DETECTION_TYPES_H

struct NearPass {
    long time; // UNIX timestamp
    int distance_cm;
    double speed_mps;
    double latitude;
    double longitude;
    int rideId;
};

struct Ride {
    int rideId;         // Primary key for Ride table
    long startTime;     // Start time (UNIX timestamp)
    long endTime;       // End time (UNIX timestamp)
};

#endif