
#ifndef NEAR_PASS_DB_H
#define NEAR_PASS_DB_H

#include <string>
#include <vector>

#include "../near_pass_detection/near_pass_detection_types.h"

// Function to open the SQLite database
// Returns SQLITE_OK on success
int db_open(const std::string db_name);

void db_close();

// Function to create the Rides table
// Returns SQLITE_OK on success
int db_create_rides_table();

// Function to create the NearPass table (with rideId as foreign key)
// Returns SQLITE_OK on success
int db_create_near_pass_table();

// Function to start a new ride
// Returns Ride ID on success, -1 on fail
int db_start_ride();

// Function to end a ride
// Returns SQLITE_OK on success
int db_end_ride();

// Returns the current ride id
int db_get_current_ride_id();

// Function to insert a new NearPass record
// Returns SQLITE_OK on success
int db_insert_near_pass(const NearPass& nearPass);

// Populates the argument ride_list with Ride objects
// Returns SQLITE_OK on success
int db_get_rides(std::vector<Ride>& ride_list);

// Populates the argument near_pass_list with NearPass objects
// Returns SQLITE_OK on success
int db_get_near_passes(std::vector<NearPass>& near_pass_list);

#endif