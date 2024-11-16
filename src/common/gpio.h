
#ifndef GPIO_H
#define GPIO_H

#include <gpiod.h>

#define RPI3B_GPIO_CHIP_NAME "gpiochip0"
#define RPI5_GPIO_CHIP_NAME "gpiochip4"

#define GPIO_CHIP_TO_USE RPI3B_GPIO_CHIP_NAME

typedef struct gpiod_line gpio_pin_t;

gpio_pin_t* gpio_get_input_pin(int gpio_num);
gpio_pin_t* gpio_get_output_pin(int gpio_num);

int gpio_read(gpio_pin_t* input_pin);
void gpio_write(gpio_pin_t* output_pin, int val);

#endif