
#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>

#define RPI3B_GPIO_CHIP_NAME "gpiochip0"
#define RPI5_GPIO_CHIP_NAME "gpiochip4"

#define GPIO_CHIP_TO_USE RPI3B_GPIO_CHIP_NAME

void gpio_initialize();

struct gpiod_line* gpio_get_input_line(int gpio_num);
struct gpiod_line* gpio_get_output_line(int gpio_num);

int gpio_read_line(struct gpiod_line* input_line);
void gpio_write_line(struct gpiod_line* output_line, int val);

#endif