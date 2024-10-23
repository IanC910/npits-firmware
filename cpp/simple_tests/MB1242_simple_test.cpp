
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include <gpiod.h>

int main() {
    printf("Starting...\n");

    // GPIO
    struct gpiod_chip *chip;
    const char *chipname = "gpiochip4";
    chip = gpiod_chip_open_by_name(chipname);

    printf("Opened GPIO chip\n");

    int ultrasonic_status_gpio_num = 17;
    struct gpiod_line *ultrasonic_status_gpio_line;
    ultrasonic_status_gpio_line = gpiod_chip_get_line(chip, ultrasonic_status_gpio_num);
    gpiod_line_request_input(ultrasonic_status_gpio_line, "ultrasonic_status");

    printf("Initialised GPIO\n");

    // I2C
    const char I2C_DEVICE[] = "/dev/i2c-1";
    const int I2C_ADDR = 0x70;

    int file = open(I2C_DEVICE, O_RDWR);
    if (file < 0) {
        printf("Error: Opening i2c port file\n");
        exit(file);
    }
    int status = ioctl(file, I2C_SLAVE, I2C_ADDR);
    if (status < 0) {
        printf("Error: Configuring i2c port");
        exit(status);
    }

    printf("Initialised I2C\n");

    // Main Loop
    while(1) {
        unsigned char buf[2];
        buf[0] = I2C_ADDR;
        buf[1] = 81;

        int res = write(file, buf, 2);
        if(res != 2) {
            printf("Error writing\n");
        }

        int ultrasonic_status = 1;
        while(ultrasonic_status) {
            usleep(1000);
            ultrasonic_status = gpiod_line_get_value(ultrasonic_status_gpio_line);
        }

        res = read(file, &buf, 2);
        if(res != 2) {
            printf("Error reading\n");
        }
        else {
            printf("%d, %d\n", buf[0], buf[1]);
        }

        usleep(100000);
    }
}