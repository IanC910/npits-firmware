
#include <sqlite3.h>
#include <string>
#include <iostream>

#include "../common/structs.h"

#include "near_pass_db.h"

int main() {
    // Open the database
    if (db_open() != SQLITE_OK) {
        return 1;
    }

    // Create the Rides and NearPass tables if they don't exist
    if (db_create_rides_table() != SQLITE_OK || db_create_near_pass_table() != SQLITE_OK) {
        db_close();
        return 1;
    }

    // Start a new ride (example)
    long startTime = 1617282000;  // Example start time (UNIX timestamp)
    int rideId = db_start_ride(startTime);
    if (rideId == -1) {
        db_close();
        return 1;
    }

    // Insert new NearPass records that reference the inserted Ride
    NearPass np1 = {37.7749, -122.4194, 500.0, 60.0, 1617282000, rideId};
    NearPass np2 = {34.0522, -118.2437, 300.0, 45.0, 1617285600, rideId};

    if (db_insert_near_pass(np1) != SQLITE_OK || db_insert_near_pass(np2) != SQLITE_OK) {
        db_close();
        return 1;
    }

    // End the ride (example)
    long endTime = 1617285600;  // Example end time (UNIX timestamp)
    if (db_end_ride(rideId, endTime) != 0) {
        db_close();
        return 1;
    }

    // Get and display all NearPass records
    if (db_get_near_passes() != SQLITE_OK) {
        db_close();
        return 1;
    }

    // Close the database
    db_close();
    return 0;
}