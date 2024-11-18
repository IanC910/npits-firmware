
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "../common/time_tools.h"
#include "../common/gpio.h"


const int ULTRASONIC_I2C_ADDR = 0x70;
const char ULTRASONIC_I2C_DEV[] = "/dev/i2c-1";

const int ULTRASONIC_STATUS_GPIO = 17;



int main() {
    printf("Starting...\n");

    // GPIO
    struct gpiod_chip* chip = gpiod_chip_open_by_name(RPI3B_GPIO_CHIP_NAME);
    printf("Opened GPIO chip\n");
    struct gpiod_line* ultrasonic_status_gpio_line = gpiod_chip_get_line(chip, ULTRASONIC_STATUS_GPIO);
    gpiod_line_request_input(ultrasonic_status_gpio_line, "ultrasonic_status");
    printf("Initialised GPIO\n");

    // I2C
    int ultrasonic_i2c_file = open(ULTRASONIC_I2C_DEV, O_RDWR);
    if (ultrasonic_i2c_file < 0) {
        printf("Error: Opening i2c port file\n");
        exit(ultrasonic_i2c_file);
    }
    int status = ioctl(ultrasonic_i2c_file, I2C_SLAVE, ULTRASONIC_I2C_ADDR);
    if (status < 0) {
        printf("Error: Configuring i2c port");
        exit(status);
    }

    printf("Initialised I2C\n");

    long long start_time_ms = get_time_ms();

    // Main Loop
    while(1) {
        unsigned char tx_buf[1];
        tx_buf[0] = 81;

        int num_bytes = write(ultrasonic_i2c_file, tx_buf, sizeof(tx_buf));
        if(num_bytes != sizeof(tx_buf)) {
            printf("Error writing\n");
        }

        while(gpiod_line_get_value(ultrasonic_status_gpio_line)) {
            sleep_ms(5);
        }

        long long end_time_ms = get_time_ms();
        long long delta_time_ms = end_time_ms - start_time_ms;
        start_time_ms = get_time_ms();

        char rx_buf[2];
        num_bytes = read(ultrasonic_i2c_file, rx_buf, sizeof(rx_buf));
        if(num_bytes != sizeof(rx_buf)) {
            printf("Error reading\n");
        }
        else {
            printf("%4d, %4d, %4d, %4lld\n", rx_buf[0], rx_buf[0] % 4, rx_buf[1], delta_time_ms);
        }

        sleep_ms(5);
    }
}