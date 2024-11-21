
#include <sqlite3.h>
#include <iostream>
#include <string>

#include "../near_pass_tracking/types.h"

#include "NearPassDB.h"

using namespace std;

// Callback function to display the result of the SELECT query
static int callback(void* data, int argc, char** argv, char** colNames) {
    for (int i = 0; i < argc; i++) {
        cout << colNames[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
    }
    cout << endl;
    return 0;
}

int db_open(sqlite3** db, const string& dbName) {
    int rc = sqlite3_open(dbName.c_str(), db);
    if (rc) {
        cerr << "Can't open database: " << sqlite3_errmsg(*db) << endl;
        return rc;
    } else {
        cout << "Opened database successfully" << endl;
    }
    return SQLITE_OK;
}

int db_create_rides_table(sqlite3* db) {
    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS Rides ("
        "rideId INTEGER PRIMARY KEY AUTOINCREMENT, "
        "startTime INTEGER, "
        "endTime INTEGER);";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (Rides table): " << errMessage << endl;
        sqlite3_free(errMessage);
        return rc;
    }
    cout << "Rides table created successfully" << endl;
    return SQLITE_OK;
}

int db_create_near_pass_table(sqlite3* db) {
    const char* createTableSQL =
        "CREATE TABLE IF NOT EXISTS NearPass ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "latitude REAL, "
        "longitude REAL, "
        "distance REAL, "
        "speed REAL, "
        "time INTEGER, "
        "rideId INTEGER, "
        "FOREIGN KEY(rideId) REFERENCES Rides(rideId));";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
    if (rc != SQLITE_OK) {
        cerr << "SQL error (NearPass table): " << errMessage << endl;
        sqlite3_free(errMessage);
        return rc;
    }
    cout << "NearPass table created successfully" << endl;
    return SQLITE_OK;
}

int db_start_ride(sqlite3* db, long startTime) {
    const char* insertSQL =
        "INSERT INTO Rides (startTime, endTime) "
        "VALUES (?, NULL);";  // endTime is NULL initially

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement (Start Ride): " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    // Bind the start time
    sqlite3_bind_int64(stmt, 1, startTime);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Execution failed (Start Ride): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    // Get the generated rideId (last inserted row id)
    int rideId = sqlite3_last_insert_rowid(db);
    cout << "Ride started successfully with rideId: " << rideId << endl;

    sqlite3_finalize(stmt);
    return rideId;
}

int db_end_ride(sqlite3* db, int rideId, long endTime) {
    const char* updateSQL =
        "UPDATE Rides SET endTime = ? WHERE rideId = ?;";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement (End Ride): " << sqlite3_errmsg(db) << endl;
        return -1;
    }

    // Bind the end time and rideId
    sqlite3_bind_int64(stmt, 1, endTime);
    sqlite3_bind_int(stmt, 2, rideId);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Execution failed (End Ride): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return -1;
    }

    cout << "Ride with rideId " << rideId << " ended successfully" << endl;

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int db_insert_near_pass(sqlite3* db, const NearPass& nearPass) {
    const char* insertSQL =
        "INSERT INTO NearPass (latitude, longitude, distance, speed, time, rideId) "
        "VALUES (?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement (NearPass): " << sqlite3_errmsg(db) << endl;
        return rc;
    }

    // Bind values to the prepared statement
    sqlite3_bind_double(stmt, 1, nearPass.latitude);
    sqlite3_bind_double(stmt, 2, nearPass.longitude);
    sqlite3_bind_double(stmt, 3, nearPass.distance);
    sqlite3_bind_double(stmt, 4, nearPass.speed);
    sqlite3_bind_int64(stmt, 5, nearPass.time);
    sqlite3_bind_int(stmt, 6, nearPass.rideId);

    // Execute the statement
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Execution failed (NearPass): " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return rc;
    }

    cout << "NearPass inserted successfully" << endl;
    sqlite3_finalize(stmt);
    return SQLITE_OK;
}

int db_get_near_passes(sqlite3* db) {
    const char* selectSQL = "SELECT * FROM NearPass;";

    char* errMessage = nullptr;
    int rc = sqlite3_exec(db, selectSQL, callback, nullptr, &errMessage);
    if (rc != SQLITE_OK) {
        cerr << "SQL error: " << errMessage << endl;
        sqlite3_free(errMessage);
        return rc;
    }
    cout << "Query executed successfully" << endl;
    return SQLITE_OK;
}
