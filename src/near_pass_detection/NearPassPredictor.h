#ifndef NEAR_PASS_VALIDATOR_H
#define NEAR_PASS_VALIDATOR_H

#include "../devices/OPS243.h"

#define MINIMUM_SPEED_THRESHOLD 6 //
#define MINIMUM_RANGE_THRESHOLD 2.5 //
#define SPEED_MAGNITUDE_THRESHOLD 25 //
#define RANGE_MAGNITUDE_THRESHOLD 300 //
#define MAXIMUM_RANGE_VALUE 2.0

class NearPassPredictor : public OPS243 {
    public:
        NearPassPredictor(const char serial_port[], int BAUD_RATE);
        ~NearPassPredictor();

        void initialize_sensor();
        void set_flag_high();
        
        bool is_vehicle_approaching(float* speed_matrix, int* magnitude_matrix);
        bool is_vehicle_in_range(float* range_matrix, int* magnitude_matrix);
        
    private:
        bool flag;
};

#endif