#include <sqlite3.h>
#include <iostream>
#include <string>

using namespace std;

// Struct to represent a NearPass record
struct NearPass {
  double latitude;
  double longitude;
  double distance;
  double speed;
  long time;  // UNIX timestamp
  int rideId;
};

// Struct to represent a Ride record
struct Ride {
  int rideId;         // Primary key for Ride table
  long startTime;     // Start time (UNIX timestamp)
  long endTime;       // End time (UNIX timestamp)
};

// Callback function to display the result of the SELECT query
static int callback(void* data, int argc, char** argv, char** colNames) {
  for (int i = 0; i < argc; i++) {
    cout << colNames[i] << ": " << (argv[i] ? argv[i] : "NULL") << endl;
  }
  cout << endl;
  return 0;
}

// Function to open the SQLite database
int openDatabase(sqlite3** db, const string& dbName) {
  int rc = sqlite3_open(dbName.c_str(), db);
  if (rc) {
    cerr << "Can't open database: " << sqlite3_errmsg(*db) << endl;
    return rc;
  } else {
    cout << "Opened database successfully" << endl;
  }
  return SQLITE_OK;
}

// Function to create the Rides table
int createRidesTable(sqlite3* db) {
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

// Function to create the NearPass table (with rideId as foreign key)
int createNearPassTable(sqlite3* db) {
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

// Function to start a new ride
int startRide(sqlite3* db, long startTime) {
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

// Function to end a ride by updating the endTime
int endRide(sqlite3* db, int rideId, long endTime) {
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
  return 0;
}

// Function to insert a new NearPass record
int insertNearPass(sqlite3* db, const NearPass& nearPass) {
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

// Function to query and display all near passes
int getNearPasses(sqlite3* db) {
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

// int main() {
//   sqlite3* db;

//   // Open the database
//   if (openDatabase(&db, "near_pass.db") != SQLITE_OK) {
//     return 1;
//   }

//   // Create the Rides and NearPass tables if they don't exist
//   if (createRidesTable(db) != SQLITE_OK || createNearPassTable(db) != SQLITE_OK) {
//     sqlite3_close(db);
//     return 1;
//   }

//   // Start a new ride (example)
//   long startTime = 1617282000;  // Example start time (UNIX timestamp)
//   int rideId = startRide(db, startTime);
//   if (rideId == -1) {
//     sqlite3_close(db);
//     return 1;
//   }

//   // Insert new NearPass records that reference the inserted Ride
//   NearPass np1 = {37.7749, -122.4194, 500.0, 60.0, 1617282000, rideId};
//   NearPass np2 = {34.0522, -118.2437, 300.0, 45.0, 1617285600, rideId};

//   if (insertNearPass(db, np1) != SQLITE_OK || insertNearPass(db, np2) != SQLITE_OK) {
//     sqlite3_close(db);
//     return 1;
//   }

//   // End the ride (example)
//   long endTime = 1617285600;  // Example end time (UNIX timestamp)
//   if (endRide(db, rideId, endTime) != 0) {
//     sqlite3_close(db);
//     return 1;
//   }

//   // Get and display all NearPass records
//   if (getNearPasses(db) != SQLITE_OK) {
//     sqlite3_close(db);
//     return 1;
//   }

//   // Close the database
//   sqlite3_close(db);
//   return 0;
// }
