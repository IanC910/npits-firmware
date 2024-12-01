
#include <stdio.h>
#include <string>

#include "../common/time_tools.h"

int main() {
    printf("Now: %ld\n", get_time_s());
    printf("%s\n", get_timestamp_hms().c_str());
    sleep_s(2);
    printf("2s later: %ld\n", get_time_s());
    printf("%s\n", get_timestamp_hms().c_str());

    set_time_s(0);

    printf("Now: %ld\n", get_time_s());
    printf("%s\n", get_timestamp_hms().c_str());
    sleep_s(2);
    printf("2s later: %ld\n", get_time_s());
    printf("%s\n", get_timestamp_hms().c_str());


}