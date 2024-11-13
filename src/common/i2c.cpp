
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "i2c.h"

int i2c_open_file(const char* i2c_device) {
    int i2c_file = open(i2c_device, O_RDWR);
    if(i2c_file < 0) {
        printf("I2C Error: Can't open I2C file: %s\n", strerror(errno));
        exit(i2c_file);
    }
    return i2c_file;
}

void i2c_close_file(int i2c_file) {
    close(i2c_file);
}

int i2c_set_address(int i2c_file, int i2c_address) {
    int result = ioctl(i2c_file, I2C_SLAVE, i2c_address);
    if(result < 0) {
        printf("I2C Error: Can't set address: %s\n", strerror(errno));
        exit(result);
    }
    return result;
}

int i2c_read(int i2c_file, char* rx_buf, int length) {
    int num_bytes = read(i2c_file, rx_buf, length);
    return (num_bytes != length);
}

int i2c_write(int i2c_file, char* tx_buf, int length) {
    int num_bytes = write(i2c_file, tx_buf, length);
    return (num_bytes != length);
}