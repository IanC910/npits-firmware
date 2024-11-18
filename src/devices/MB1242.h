
#ifndef MB1242_H
#define MB1242_H

#include <thread>

#include "../common/gpio.h"



const int MB1242_I2C_ADDRESS = 0x70;
const int MB1242_TAKE_READING_CMD_ID = 81;

const int MB1242_MAX_DISTANCE_cm = 765;

class MB1242 {
public:
    MB1242(const char* i2c_device, int status_gpio_num);
    ~MB1242();



    // === Asynchronous reading ===

    // Starts the sampling thread
    void begin_sampling();

    // Stops the sampling thread
    void stop_sampling();

    bool is_new_distance_available();

    // Returns the latest distance reading
    int get_latest_distance_cm();



    // === Direct control ===

    // Initiates a distance reading
    // Returns 0 on success
    int initiate_distance_reading();

    bool is_reading_in_progress();

    // Updates the latest distance report in cm
    // Returns 0 if success
    int update_distance_report();



protected:
    bool do_run_sampler = false;
    std::thread sampler_thread;

    int latest_distance_cm = 0;
    int is_new_sample = false;

    int i2c_file = 0;
    int i2c_address = 0;

    gpio_pin status_gpio_pin = GPIO_NO_PIN;

    void run_distance_sampler();
};

#endif