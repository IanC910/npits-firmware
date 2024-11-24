#ifndef VehicleApproachDetector_H
#define VehicleApproachDetector_H

#include "OPS243.h"

#define unsigned int SPEED_MASK = 0x0000FF00
#define unsigned int RANGE_MASK = 0xFF000000

#define unsigned int SPEED_VALUE_MASK = 0x000000FF 
#define unsigned int RANGE_VALUE_MASK = 0x00FF0000

#define unsigned int EXPECTED_SPEED_MASK = 0x00000100
#define unsigned int EXPECTED_RANGE_MASK = 0x02000000

#define unsigned int MINIMUM_SPEED_THRESHOLD
#define unsigned int MAXIMUM_RANGE_THRESHOLD
#define unsigned int SPEED_MAGNITUDE_THRESHOLD
#define unsigned int RANGE_MAGNITUDE_THRESHOLD

#define unsigned int MAXIMUM_RANGE_VALUE 1.5
class VehicleApproachDetector : public OPS243 {
    public:
        static const int SPEED_THRESHOLD = 12;
        static const int MAGNITUDE_THRESHOLD = 12;

        VehicleApproachDetector();
        ~VehicleApproachDetector();

        void initialize_sensor();

        void set_flag_high(bool flag_val);
        void get_flag_value(unsigned int val);

        bool is_vehicle_approaching();
        int check_range();
        void raise_flag();

    private :
        bool flag;
};