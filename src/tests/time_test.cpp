
#include <stdio.h>
#include <string>

#include "../common/time_tools.h"
#include "../common/log.h"

int main() {
    log("time_test", "Now: " + std::to_string((int)get_time_s()));
    sleep_s(2);
    log("time_test", "2s later: " + std::to_string(get_time_s()));

    set_time_s(0);

    log("time_test", "Now: " + std::to_string((int)get_time_s()));
    sleep_s(2);
    log("time_test", "2s later: " + std::to_string(get_time_s()));
}