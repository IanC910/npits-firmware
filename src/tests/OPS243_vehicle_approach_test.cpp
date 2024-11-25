#include "near_pass_validator.cpp"
#include <iostream>

int main() {
    const char serial_port[] = "/dev/ttyACM0";  // Example serial port
    int baud_rate = 115200;  // Example baud rate

    near_pass_validator sensor(serial_port, baud_rate);

    // Initialize the sensor with custom settings
    sensor.initialize_sensor();

    int speed_magnitudes[9];
    int range_magnitudes[9];
    float speeds[9];
    float ranges[9];

    while (true) {
        // Assuming read_serial_data reads sensor data and populates the arrays
        read_serial_file(sensor, speed_magnitudes, range_magnitudes, speeds, ranges);

	if(sensor.is_vehicle_approaching(speeds, speed_magnitudes))
		sensor.is_vehicle_in_range(ranges, range_magnitudes);
                    //    std::cout << "Vehicle is approaching!" << std::endl;
                    }
    
        // Add a small delay or break condition if necessary to avoid busy looping
    

    return 0;
   
}
