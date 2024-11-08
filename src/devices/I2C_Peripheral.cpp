
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "I2C_Peripheral.h"

I2C_Peripheral::I2C_Peripheral(const char* i2c_device, const int i2c_address) {
    this->i2c_address = i2c_address;

    i2c_file = open(i2c_device, O_RDWR);
    if(i2c_file < 0) {
        printf("I2C_Peripheral Error: Can't open i2c file\n");
        exit(i2c_file);
    }
    int result = ioctl(i2c_file, I2C_SLAVE, i2c_address);
    if(result < 0) {
        printf("I2C_Peripheral Error: Can't configure i2c port\n");
        exit(result);
    }
}

I2C_Peripheral::~I2C_Peripheral() {
    close(i2c_file);
}

int I2C_Peripheral::i2c_read(char* rx_buf, int length) {
    int num_bytes = read(i2c_file, rx_buf, length);
    return (num_bytes != length);
}

int I2C_Peripheral::i2c_write(char* tx_buf, int length) {
    int num_bytes = write(i2c_file, tx_buf, length);
    return (num_bytes != length);
}