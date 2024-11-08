
#ifndef MB1242_H
#define MB1242_H

#include "I2C_Peripheral.h"
#include "GPIO.h"

const int MB1242_I2C_ADDRESS = 0x70;
const int MB1242_TAKE_READING_CMD_ID = 81;

class MB1242 : public I2C_Peripheral {
public:
    MB1242(const char* i2c_device, int status_gpio_num);

    // Initiates a distance reading
    // Returns 0 if successful
    int start_distance_reading();

    bool is_reading_in_progress();

    // Returns the latest distance report in cm
    // Returns -1 if failed
    int get_distance_report_cm();

private:
    struct gpiod_line* status_line = nullptr;
};

#endif