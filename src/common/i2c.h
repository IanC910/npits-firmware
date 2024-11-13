
#ifndef I2C_H
#define I2C_H

// Return file descriptor
int i2c_open_file(const char* i2c_device);

void i2c_close_file(int i2c_file);

// Returns 0 on success
int i2c_set_address(int i2c_file, int i2c_address);

// Return 0 on success
int i2c_read(int i2c_file, char* rx_buf, int length);

// Return 0 on success
int i2c_write(int i2c_file, char* tx_buf, int length);

#endif