
#ifndef NEAR_PASS_DB_H
#define NEAR_PASS_DB_H

#include <sqlite3.h>
#include <string>

#include "../near_pass_tracking/types.h"

// Function to open the SQLite database
// Returns SQLITE_OK on success
int db_open();

int db_close();

// Function to create the Rides table
// Returns SQLITE_OK on success
int db_create_rides_table();

// Function to create the NearPass table (with rideId as foreign key)
// Returns SQLITE_OK on success
int db_create_near_pass_table();

// Function to start a new ride
// Returns Ride ID on success, -1 on fail
int db_start_ride(long startTime);

// Function to end a ride by updating the endTime
// Returns SQLITE_OK on success
int db_end_ride(int rideId, long endTime);

// Function to insert a new NearPass record
// Returns SQLITE_OK on success
int db_insert_near_pass(const NearPass& nearPass);

// Function to query and display all near passes
// Returns SQLITE_OK on success
int db_get_near_passes();

#endif