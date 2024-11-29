#ifndef NEAR_PASS_PREDICTOR_H
#define NEAR_PASS_PREDICTOR_H

#include "../devices/OPS243.h"

class NearPassPredictor {
public:
    NearPassPredictor(OPS243* radar);
    ~NearPassPredictor();

    void initialize_radar();
    
    void update_speeds_and_ranges();
    bool is_vehicle_approaching();
    bool is_vehicle_in_range();
    
private:
    bool flag;

    float range_m_array[OPS243::MAX_REPORTS];
    float range_magnitude_array[OPS243::MAX_REPORTS];

    float speed_mps_array[OPS243::MAX_REPORTS];
    float speed_magnitude_array[OPS243::MAX_REPORTS];

    OPS243* radar = nullptr;
};

#endif