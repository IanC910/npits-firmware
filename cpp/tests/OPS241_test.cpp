
#include <stdio.h>
#include <unistd.h>

#include "../devices/OPS241.h"

int main() {
    const char SERIAL_PORT[] = "/dev/ttyACM0";
    OPS241 radar(SERIAL_PORT);

    radar.stop_reporting_distance();

    char module_info[512];
    int num_bytes = radar.get_module_info(module_info, sizeof(module_info));
    printf("Module Info Size: %d B\n", num_bytes);
    printf(module_info);

    radar.start_reporting_distance();

    while(1) {
        char reading[32];
        radar.read_buffer(reading, sizeof(reading));
        printf(reading);
    }
}