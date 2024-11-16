
#ifndef MB1242_H
#define MB1242_H

#include "../common/gpio.h"

const int MB1242_I2C_ADDRESS = 0x70;
const int MB1242_TAKE_READING_CMD_ID = 81;

const int MB1242_MAX_DISTANCE_cm = 765;

class MB1242 {
public:
    MB1242(const char* i2c_device, int status_gpio_num);
    ~MB1242();

    // Initiates a distance reading
    // Returns 0 if successful
    int start_distance_reading();

    bool is_reading_in_progress();

    // Returns the latest distance report in cm
    // Returns -1 if failed
    int get_distance_report_cm();

protected:
    int i2c_file = 0;
    int i2c_address = 0;

    gpio_pin status_gpio_pin = GPIO_NO_PIN;
};

#endif