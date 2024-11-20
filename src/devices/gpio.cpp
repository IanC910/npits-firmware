
#include "gpio.h"

static struct gpiod_chip* gpio_chip = nullptr;

gpio_pin gpio_get_input_pin(int gpio_num) {
    if(gpio_chip == nullptr) {
        gpio_chip = gpiod_chip_open_by_name(GPIO_CHIP_TO_USE);
    }

    gpio_pin input_pin;
    input_pin = gpiod_chip_get_line(gpio_chip, gpio_num);
    gpiod_line_request_input(input_pin, "input");
    return input_pin;
}

gpio_pin gpio_get_output_pin(int gpio_num) {
    if(gpio_chip == nullptr) {
        gpio_chip = gpiod_chip_open_by_name(GPIO_CHIP_TO_USE);
    }

    gpio_pin output_pin;
    output_pin = gpiod_chip_get_line(gpio_chip, gpio_num);
    gpiod_line_request_output(output_pin, "output", 0);
    return output_pin;
}

int gpio_read(gpio_pin input_pin) {
    return gpiod_line_get_value(input_pin);
}

void gpio_write(gpio_pin output_pin, int value) {
    gpiod_line_set_value(output_pin, value);
}