
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "GPIO.h"

#include "MB1242.h"

MB1242::MB1242(const char* i2c_device, int status_gpio_num) {
    // I2C
    i2c_file = open(i2c_device, O_RDWR);
    if(i2c_file < 0) {
        printf("MB1242 Error: Opening i2c port file\n");
        exit(i2c_file);
    }
    int status = ioctl(i2c_file, I2C_SLAVE, MB1242_I2C_ADDRESS);
    if(status < 0) {
        printf("MB1242 Error: Configuring i2c port\n");
        exit(status);
    }

    // GPIO
    GPIO_initialize();
    status_line = GPIO_get_input_line(status_gpio_num);
}

MB1242::~MB1242() {
    close(i2c_file);
}

int MB1242::start_distance_reading() {
    char tx_buf[2];
    tx_buf[0] = MB1242_I2C_ADDRESS;
    tx_buf[1] = MB1242_TAKE_READING_CMD_ID;
    int num_bytes = write(i2c_file, tx_buf, sizeof(tx_buf));
    return (num_bytes != sizeof(tx_buf));
}

bool MB1242::is_reading_in_progress() {
    return GPIO_read_line(status_line);
}

int MB1242::get_distance_report_cm() {
    if(is_reading_in_progress()) {
        return -1;
    }

    char rx_buf[2];
    int num_bytes = read(i2c_file, rx_buf, sizeof(rx_buf));
    if(num_bytes != sizeof(rx_buf)) {
        return -1;
    }

    return rx_buf[0] * 256 + rx_buf[1];
}