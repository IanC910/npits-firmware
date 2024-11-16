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

// Function to create the NearPass table
int createTable(sqlite3* db) {
  const char* createTableSQL =
      "CREATE TABLE IF NOT EXISTS NearPass ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT, "
      "latitude REAL, "
      "longitude REAL, "
      "distance REAL, "
      "speed REAL, "
      "time INTEGER, "
      "rideId INTEGER);";

  char* errMessage = nullptr;
  int rc = sqlite3_exec(db, createTableSQL, 0, 0, &errMessage);
  if (rc != SQLITE_OK) {
    cerr << "SQL error: " << errMessage << endl;
    sqlite3_free(errMessage);
    return rc;
  }
  cout << "Table created successfully" << endl;
  return SQLITE_OK;
}

// Function to insert a new NearPass record
int insertNearPass(sqlite3* db, const NearPass& nearPass) {
  const char* insertSQL =
      "INSERT INTO NearPass (latitude, longitude, distance, speed, time, "
      "rideId) "
      "VALUES (?, ?, ?, ?, ?, ?);";

  sqlite3_stmt* stmt;
  int rc = sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr);
  if (rc != SQLITE_OK) {
    cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << endl;
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
    cerr << "Execution failed: " << sqlite3_errmsg(db) << endl;
    sqlite3_finalize(stmt);
    return rc;
  }

  cout << "Data inserted successfully" << endl;
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

int main() {
  sqlite3* db;

  // Open the database
  if (openDatabase(&db, "near_pass.db") != SQLITE_OK) {
    return 1;
  }

  // Create the table if it doesn't exist
  if (createTable(db) != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  // Insert new NearPass records (example usage)
  NearPass np1 = {37.7749, -122.4194, 500.0, 60.0, 1617282000, 1};
  NearPass np2 = {34.0522, -118.2437, 300.0, 45.0, 1617285600, 2};

  if (insertNearPass(db, np1) != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  if (insertNearPass(db, np2) != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  // Get and display all NearPass records
  if (getNearPasses(db) != SQLITE_OK) {
    sqlite3_close(db);
    return 1;
  }

  // Close the database
  sqlite3_close(db);
  return 0;
}
