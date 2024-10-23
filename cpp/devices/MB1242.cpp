
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "I2C_Peripheral.h"
#include "GPIO.h"

#include "MB1242.h"

MB1242::MB1242(const char* i2c_device, int status_gpio_num)
    : I2C_Peripheral(i2c_device, MB1242_I2C_ADDRESS)
{
    // GPIO
    GPIO_initialize();
    status_line = GPIO_get_input_line(status_gpio_num);
}

int MB1242::start_distance_reading() {
    char tx_buf[] = {MB1242_TAKE_READING_CMD_ID};
    int result = i2c_write(tx_buf, sizeof(tx_buf));
    return result;
}

bool MB1242::is_reading_in_progress() {
    return GPIO_read_line(status_line);
}

int MB1242::get_distance_report_cm() {
    if(is_reading_in_progress()) {
        return -1;
    }

    char rx_buf[2];
    int result = i2c_read(rx_buf, sizeof(rx_buf));
    if(result) {
        return -1;
    }

    return rx_buf[0] * 256 + rx_buf[1];
}