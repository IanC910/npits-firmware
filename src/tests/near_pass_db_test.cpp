
#include <sqlite3.h>

#include <iostream>
#include <string>
#include <vector>

#include "../near_pass_detection/near_pass_detection_types.h"

#include "../db/near_pass_db.h"

int main() {
    // Open the database
    if (db_open("near_pass.db") != SQLITE_OK) {
        return 1;
    }

    // Create the Rides and NearPass tables if they don't exist
    if (db_create_rides_table() != SQLITE_OK || db_create_near_pass_table() != SQLITE_OK) {
        db_close();
        return 1;
    }

    // Start a new ride (example)
    int rideId = db_start_ride();
    if (rideId == -1) {
        db_close();
        return 1;
    }

    // Insert new NearPass records that reference the inserted Ride
    NearPass np1 = {1617282000, 500, 60.0, 37.7749, -122.4194, rideId};
    NearPass np2 = {1617285600, 300, 45.0, 34.0522, -118.2437, rideId};

    if (db_insert_near_pass(np1) != SQLITE_OK || db_insert_near_pass(np2) != SQLITE_OK) {
        db_close();
        return 1;
    }

    // End the ride (example)
    if (db_end_ride() != 0) {
        db_close();
        return 1;
    }

    int rideId2 = db_start_ride();
    if (rideId2 == -1) {
        db_close();
        return 1;
    }

    if(db_end_ride() != 0) {
        db_close();
        return 1;
    }

    std::vector<Ride> ride_list;
    if(db_get_rides(ride_list) != SQLITE_OK) {
        db_close();
        return 1;
    }

    printf("Rides:\n");
    for(int i = 0; i < ride_list.size(); i++) {
        Ride ride = ride_list[i];

        printf(
            "Ride ID:       %d\n"
            "Start Time:    %ld\n"
            "End Time:      %ld\n"
            "\n",
            ride.rideId,
            ride.startTime,
            ride.endTime
        );
    }

    // Get and display all NearPass records
    std::vector<NearPass> near_pass_list;
    near_pass_list.reserve(10);
    if (db_get_near_passes(near_pass_list) != SQLITE_OK) {
        db_close();
        return 1;
    }

    printf("Near Passes:\n");
    for(int i = 0; i < near_pass_list.size(); i++) {
        NearPass near_pass = near_pass_list[i];

        printf(
            "distance_cm: %d\n"
            "speed_mps:   %f\n"
            "latitude:    %lf\n"
            "longitude:   %lf\n"
            "ride id:     %d\n"
            "\n",
            near_pass.distance_cm,
            near_pass.speed_mps,
            near_pass.latitude,
            near_pass.longitude,
            near_pass.rideId
        );
    }

    // Close the database
    db_close();
    return 0;
}