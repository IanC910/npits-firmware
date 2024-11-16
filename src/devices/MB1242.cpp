
#include <stdio.h>

#include "../common/time_tools.h"
#include "../common/gpio.h"
#include "../common/i2c.h"

#include "MB1242.h"

MB1242::MB1242(const char* i2c_device, int status_gpio_num) {
    this->i2c_file = i2c_open_file(i2c_device);
    status_gpio_pin = gpio_get_input_pin(status_gpio_num);
}

MB1242::~MB1242() {
    i2c_close_file(i2c_file);
}

int MB1242::start_distance_reading() {
    i2c_set_address(i2c_file, MB1242_I2C_ADDRESS);

    char tx_buf[] = {MB1242_TAKE_READING_CMD_ID};
    int result = i2c_write(i2c_file, tx_buf, sizeof(tx_buf));
    return result;
}

bool MB1242::is_reading_in_progress() {
    return gpio_read(status_gpio_pin);
}

int MB1242::get_distance_report_cm() {
    if(is_reading_in_progress()) {
        return -1;
    }

    i2c_set_address(i2c_file, MB1242_I2C_ADDRESS);

    char rx_buf[2];
    int result = i2c_read(i2c_file, rx_buf, sizeof(rx_buf));
    if(result) {
        return -1;
    }

    return rx_buf[0] * 256 + rx_buf[1];
}