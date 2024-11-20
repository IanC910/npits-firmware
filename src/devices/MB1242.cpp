
#include <stdio.h>
#include <thread>
#include <chrono>
#include <string>

#include "../common/time_tools.h"
#include "gpio.h"
#include "i2c.h"

#include "MB1242.h"



MB1242::MB1242(const std::string i2c_device, int status_gpio_num) {
    this->i2c_file = i2c_open_file(i2c_device.c_str());
    status_gpio_pin = gpio_get_input_pin(status_gpio_num);
}

MB1242::~MB1242() {
    stop_sampling();
    i2c_close_file(i2c_file);
}

// Async

void MB1242::begin_sampling() {
    do_run_sampler = true;
    sampler_thread = std::thread(&MB1242::run_sampler, this);
}

void MB1242::stop_sampling() {
    do_run_sampler = false;
    if(sampler_thread.joinable()) {
        sampler_thread.join();
    }
}

bool MB1242::is_new_report_available() {
    return is_new_sample;
}

MB1242::report MB1242::get_latest_report() {
    is_new_sample = false;
    return latest_report;
}

// Direct Control

int MB1242::initiate_distance_reading() {
    i2c_set_address(i2c_file, MB1242_I2C_ADDRESS);

    char tx_buf[] = {MB1242_TAKE_READING_CMD_ID};
    int result = i2c_write(i2c_file, tx_buf, sizeof(tx_buf));
    return result;
}

bool MB1242::is_reading_in_progress() {
    return gpio_read(status_gpio_pin);
}

int MB1242::update_report() {
    if(is_reading_in_progress()) {
        return -1;
    }

    i2c_set_address(i2c_file, MB1242_I2C_ADDRESS);

    char rx_buf[2];
    int result = i2c_read(i2c_file, rx_buf, sizeof(rx_buf));
    if(result) {
        return result;
    }

    int distance_cm = (rx_buf[0] % 4) * 256 + rx_buf[1];
    latest_report.distance_cm = distance_cm;
    latest_report.time_stamp_ms = get_time_ms();
    is_new_sample = true;
    return 0;
}

void MB1242::run_sampler() {
    while(do_run_sampler) {
        initiate_distance_reading();

        while(is_reading_in_progress()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        }

        update_report();
    }
}