
#include <stdio.h>

#include "../common/time_tools.h"

int main() {
    printf("Now: %ld\n", get_time_s());
    sleep_s(2);
    printf("2s later: %ld\n", get_time_s());

    set_time_s(0);

    printf("Now: %ld\n", get_time_s());
    sleep_s(2);
    printf("2s later: %ld\n", get_time_s());
}