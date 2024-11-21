
#ifndef STRUCTS_H
#define STRUCTS_H

struct NearPass {
    int rideId;
    long time; // UNIX timestamp
    int distance_cm;
    double speed;
    double latitude;
    double longitude;
};

struct Ride {
    int rideId;         // Primary key for Ride table
    long startTime;     // Start time (UNIX timestamp)
    long endTime;       // End time (UNIX timestamp)
};

#endif