
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <gpiod.h>

int main() {
    // GPIO
    struct gpiod_chip *chip;
    const char *chipname = "gpiochip0";
    chip = gpiod_chip_open_by_name(chipname);

    printf("Opened GPIO chip\n");

    struct gpiod_line *test_line;
    test_line = gpiod_chip_get_line(chip, test_gpio_num);
    gpiod_line_request_input(test_line, "test_line");

    printf("Initialised GPIO\n");

    while(1) {
        int val = gpiod_line_get_value(test_line);
        printf("%d\n", val);
        usleep(10000);
    }
}