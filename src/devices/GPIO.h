
#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>

const char RPI5_GPIO_CHIP_NAME[] = "gpiochip4";

void GPIO_initialize();

struct gpiod_line* GPIO_get_input_line(int gpio_num);
struct gpiod_line* GPIO_get_output_line(int gpio_num);

int GPIO_read_line(struct gpiod_line* input_line);
void GPIO_write_line(struct gpiod_line* output_line, int val);

#endif