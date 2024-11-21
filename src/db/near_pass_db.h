
#ifndef NEAR_PASS_DB_H
#define NEAR_PASS_DB_H

#include <sqlite3.h>
#include <string>

#include "../near_pass_tracking/types.h"

// Function to open the SQLite database
// Sets *db to a pointer to a db
// Returns SQLITE_OK on success
int db_open(sqlite3** db, const string& dbName);

// Function to create the Rides table
// Returns SQLITE_OK on success
int db_create_rides_table(sqlite3* db);

// Function to create the NearPass table (with rideId as foreign key)
// Returns SQLITE_OK on success
int db_create_near_pass_table(sqlite3* db);

// Function to start a new ride
// Returns Ride ID on success, -1 on fail
int db_start_ride(sqlite3* db, long startTime);

// Function to end a ride by updating the endTime
// Returns SQLITE_OK on success
int db_end_ride(sqlite3* db, int rideId, long endTime);

// Function to insert a new NearPass record
// Returns SQLITE_OK on success
int db_insert_near_pass(sqlite3* db, const NearPass& nearPass);

// Function to query and display all near passes
// Returns SQLITE_OK on success
int db_get_near_passes(sqlite3* db);

#endif