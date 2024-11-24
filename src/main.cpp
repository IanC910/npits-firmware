
#include "common/time_tools.h"
#include "bluetooth/le_server.h"

#include <string>

char DEVICE_FILE[] = "bluetooth/devices.txt";

int main() {
    le_server_start(DEVICE_FILE);

    while(1) {
        sleep_s(100);
    }
}