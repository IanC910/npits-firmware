// SOURCE: https://www.kernel.org/doc/Documentation/i2c/dev-interface */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

unsigned char readI2CReg(int i2cFileDesc, unsigned char addr) {
    // To read a register, must first write the address
    int res = write(i2cFileDesc, &addr, sizeof(addr));

    if (res != sizeof(addr)) {
        perror("I2C: Unable to write to i2c register.");
        exit(1);
    }

    // Now read the value and return it
    char value = 0;
    res = read(i2cFileDesc, &value, sizeof(value));

    if (res != sizeof(value)) {
        perror("I2C: Unable to read from i2c register");
        exit(1);
    }

    return value;
}

void writeI2CReg(int i2cFileDesc, unsigned char addr, unsigned char value) {
    unsigned char buff[2];

    buff[0] = addr;
    buff[1] = value;

    int res = write(i2cFileDesc, buff, 2);

    if (res != 2) {
        perror("I2C: Unable to write i2c register.");
        exit(1);
    }
}

int main() {
    int file;
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-1");
    int addr = 0x70;

    file = open(filename, O_RDWR);
    if (file < 0) {
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        exit(1);
    }

    while(1) {
        int res = write(file, &addr, sizeof(addr));
        if(res != sizeof(addr)) {
            printf("Error writing\n");
        }
        sleep(2);

        int buf[2];
        res = read(file, &buf, 2);
        if(res != 2) {
            printf("Error reading");
        }
        else {
            printf("%d, %d\n", buf[0], buf[1]);
        }

        sleep(2);

    }
}