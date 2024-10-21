// SOURCE: https://www.kernel.org/doc/Documentation/i2c/dev-interface */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

int main() {
    char filename[20];
    snprintf(filename, 19, "/dev/i2c-1");
    int addr = 0x70;

    int file = open(filename, O_RDWR);
    if (file < 0) {
        exit(1);
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0) {
        exit(1);
    }

    while(1) {
        unsigned char buf[2];
        buf[0] = addr;
        buf[1] = 81;
        int res = write(file, buf, 2);
        if(res != 2) {
            printf("Error writing\n");
        }
        sleep(2);

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