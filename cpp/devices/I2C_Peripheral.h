
#ifndef I2C_PERIPHERAL_H
#define I2C_PERIPHERAL_H

class I2C_Peripheral {
public:
    I2C_Peripheral(const char* i2c_device, const int i2c_address);
    ~I2C_Peripheral();

    // Returns 0 on success
    int i2c_read(char* rx_buf, int length);

    // Returns 0 on success
    int i2c_write(char* tx_buf, int length);

protected:
    int i2c_file = 0;
    int i2c_address = 0;
};


#endif