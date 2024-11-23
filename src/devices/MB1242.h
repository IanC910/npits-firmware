
#ifndef MB1242_H
#define MB1242_H

#include <thread>
#include <string>

#include "gpio.h"



const int MB1242_I2C_ADDRESS = 0x70;
const int MB1242_TAKE_READING_CMD_ID = 81;

const int MB1242_MAX_DISTANCE_cm = 765;

class MB1242 {
public:
    MB1242(const std::string i2c_device, int status_gpio_num);
    ~MB1242();

    struct report {
        long long time_stamp_ms;
        unsigned short distance_cm;
    };

    // === Asynchronous reading ===

    // Starts the sampling thread
    void begin_sampling();

    // Stops the sampling thread
    void stop_sampling();

    bool is_new_report_available();

    report get_latest_report();



    // === Direct control ===

    // Initiates a distance reading
    // Returns 0 on success
    int initiate_distance_reading();

    bool is_reading_in_progress();

    // Updates the latest distance report in cm
    // Returns 0 on success
    int update_report();



protected:
    report latest_report = {0, 0};
    int is_new_sample = false;

    int i2c_file = 0;
    int i2c_address = 0;
    gpio_pin status_gpio_pin = GPIO_NO_PIN;

    bool do_run_sampler = false;
    std::thread* sampler_thread;
    void run_sampler();
};

#endif