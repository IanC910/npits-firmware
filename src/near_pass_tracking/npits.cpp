
#include <stdio.h>
#include <string>

#include "../bluetooth/le_server.h"
#include "near_pass_detection.h"

#include "npits.h"

// Inits all modules that need to be initialized before starting the le_server
static void npits_init() {
    near_pass_detection_init(ULTRASONIC_I2C_DEVICE, ULTRASONIC_STATUS_GPIO_NUM);
}

void npits_run() {
    printf("Starting NPITS");
    npits_init();

    le_server_start();

    // le_server_stop();
}