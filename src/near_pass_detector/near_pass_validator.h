#ifndef NEAR_PASS_VALIDATOR_H
#define NEAR_PASS_VALIDATOR_H

#include "OPS243.h"

#define MINIMUM_SPEED_THRESHOLD 0.5
#define MAXIMUM_RANGE_THRESHOLD 3
#define SPEED_MAGNITUDE_THRESHOLD 500
#define RANGE_MAGNITUDE_THRESHOLD 300
#define MAXIMUM_RANGE_VALUE 2.0

class near_pass_validator : public OPS243 {
    public:
        near_pass_validator(const char serial_port[], int BAUD_RATE);
        ~near_pass_validator();

        void initialize_sensor();
        void set_flag_high();
        
        bool is_vehicle_approaching(float* speed_matrix, int* magnitude_matrix);
        bool is_vehicle_in_range(float* range_matrix, int* magnitude_matrix);
        
    private:
        bool flag;
};

#endif

