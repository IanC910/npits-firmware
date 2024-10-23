
#include "GPIO.h"

static struct gpiod_chip* gpio_chip = nullptr;

void GPIO_initialize() {
    if(gpio_chip == nullptr) {
        gpio_chip = gpiod_chip_open_by_name(RPI5_GPIO_CHIP_NAME);
    }
}

struct gpiod_line* GPIO_get_input_line(int gpio_num) {
    struct gpiod_line* input_line;
    input_line = gpiod_chip_get_line(gpio_chip, gpio_num);
    gpiod_line_request_input(input_line, "input");
    return input_line;
}

struct gpiod_line* GPIO_get_output_line(int gpio_num) {
    struct gpiod_line* output_line;
    output_line = gpiod_chip_get_line(gpio_chip, gpio_num);
    gpiod_line_request_output(output_line, "output", 0);
    return output_line;
}

int GPIO_read_line(struct gpiod_line* input_line) {
    return gpiod_line_get_value(input_line);
}

void GPIO_write_line(struct gpiod_line* output_line, int val) {
    gpiod_line_set_value(output_line, val);
}