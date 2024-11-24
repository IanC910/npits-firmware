
#include <sqlite3.h>
#include <iostream>
#include <string>
#include <vector>

#include "../near_pass_detector/near_pass_detector_types.h"

#include "near_pass_db.h"

static sqlite3* db;
static const std::string DB_NAME = "near_pass.db";

static std::vector<Ride>* current_ride_list;
static std::vector<NearPass>* current_near_pass_list;

int db_open() {
    int rc = sqlite3_open(DB_NAME.c_str(), &db);
    if (rc) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return rc;
    } else {
        std::cout << "Opened database successfully" << std::endl;
    }
    return SQLITE_OK;
}

void db_close() {
    sqlite3_close(db);
}

int db_create_rides_table() {
    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS Rides ("
        "rideId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "startTime INTEGER, "
        "endTime INTEGER);";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error (Rides table): " << errMessage << std::endl;
        sqlite3_free(errMessage);
        return rc;
    }
    std::cout << "Rides table created successfully" << std::endl;
    return SQLITE_OK;
}

int db_create_near_pass_table() {
    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS NearPass ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "time INTEGER, "
        "distance_cm INTEGER, "
        "speed_mps REAL, "
        "latitude REAL, "
        "longitude REAL, "
        "rideId INTEGER, "
        "FOREIGN KEY(rideId) REFERENCES Rides(rideId));";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error (NearPass table): " << errMessage << std::endl;
        sqlite3_free(errMessage);
        return rc;
    }
    std::cout << "NearPass table created successfully" << std::endl;
    return SQLITE_OK;
}

int db_start_ride(long startTime) {
    const char* insertSQL =
        "INSERT INTO Rides (startTime, endTime) "
        "VALUES (?, NULL);";  // endTime is NULL initially

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare statement (Start Ride): " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Bind the start time
    sqlite3_bind_int64(stmt, 1, startTime);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cout << "Execution failed (Start Ride): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Get the generated rideId (last inserted row id)
    int rideId = sqlite3_last_insert_rowid(db);
    std::cout << "Ride started successfully with rideId: " << rideId << std::endl;

    sqlite3_finalize(stmt);
    return rideId;
}

int db_end_ride(int rideId, long endTime) {
    const char* updateSQL =
        "UPDATE Rides SET endTime = ? WHERE rideId = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare statement (End Ride): " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }

    // Bind the end time and rideId
    sqlite3_bind_int64(stmt, 1, endTime);
    sqlite3_bind_int(stmt, 2, rideId);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cout << "Execution failed (End Ride): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    std::cout << "Ride with rideId " << rideId << " ended successfully" << std::endl;

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int db_insert_near_pass(const NearPass& nearPass) {
    const char* insertSQL =
        "INSERT INTO NearPass (time, distance_cm, speed_mps, latitude, longitude, rideId) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cout << "Failed to prepare statement (NearPass): " << sqlite3_errmsg(db) << std::endl;
        return rc;
    }

    // Bind values to the prepared statement
    sqlite3_bind_double(stmt, 1, nearPass.time);
    sqlite3_bind_double(stmt, 2, nearPass.distance_cm);
    sqlite3_bind_double(stmt, 3, nearPass.speed_mps);
    sqlite3_bind_double(stmt, 4, nearPass.latitude);
    sqlite3_bind_int64(stmt, 5, nearPass.longitude);
    sqlite3_bind_int(stmt, 6, nearPass.rideId);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cout << "Execution failed (NearPass): " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    std::cout << "NearPass inserted successfully" << std::endl;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

static int get_rides_callback(void* data, int argc, char** argv, char** colNames) {
    Ride ride;
    ride.rideId     = atoi(argv[0]);
    ride.startTime  = strtol(argv[1], nullptr, 10);
    ride.endTime    = strtol(argv[2], nullptr, 10);

    current_ride_list->push_back(ride);

    return 0;
}

int db_get_rides(std::vector<Ride>& ride_list) {
    current_ride_list = &ride_list;
    current_ride_list->clear();

    const char* selectSQL = "SELECT * FROM Rides;";
    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, selectSQL, get_rides_callback, nullptr, &errMessage);
    if(rc != SQLITE_OK) {
        std::cout << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage);
        return rc;
    }
    std::cout << "Rides query executed successfully" << std::endl;
    return SQLITE_OK;
}

static int get_near_passes_callback(void* data, int argc, char** argv, char** colNames) {
    NearPass near_pass;
    near_pass.time          = strtol(argv[1], nullptr, 10);
    near_pass.distance_cm   = atoi(argv[2]);
    near_pass.speed_mps     = strtod(argv[3], nullptr);
    near_pass.latitude      = strtod(argv[4], nullptr);
    near_pass.longitude     = strtod(argv[5], nullptr);
    near_pass.rideId        = atoi(argv[6]);

    current_near_pass_list->push_back(near_pass);

    return 0;
}

int db_get_near_passes(std::vector<NearPass>& near_pass_list) {
    current_near_pass_list = &near_pass_list;
    current_near_pass_list->clear();

    const char* selectSQL = "SELECT * FROM NearPass;";
    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, selectSQL, get_near_passes_callback, nullptr, &errMessage);
    if (rc != SQLITE_OK) {
        std::cout << "SQL error: " << errMessage << std::endl;
        sqlite3_free(errMessage);
        return rc;
    }
    std::cout << "Near passes query executed successfully" << std::endl;
    return SQLITE_OK;
}
