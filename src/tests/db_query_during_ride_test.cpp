
#include "../common/log.h"

#include "../near_pass_detection/NearPassDetector.h"
#include "../near_pass_detection/near_pass_detection_types.h"

#include "../db/near_pass_db.h"

int main() {
    db_open_and_make_tables("test.db");

    db_start_ride();

    std::vector<Ride> rides_list;
    db_get_rides(rides_list);

    db_end_ride();
}