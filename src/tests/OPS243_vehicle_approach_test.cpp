#include "near_pass_validator.cpp"
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

// Function to check for keyboard 'q' press
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }

    return 0;
}

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
    
    struct timespec now;
    time_t seconds;
    int milliseconds;
    struct tm *timeinfo;
    char time_buffer[30];

    while (true) {
        if (kbhit()) {
            char ch = getchar();
            if (ch == 'q' || ch == 'Q') {
                std::cout << "Exiting program..." << std::endl;
                break;
            }
        }

	timespec_get(&now, TIME_UTC);
	seconds = now.tv_sec;
	milliseconds = now.tv_sec/1000000;
	timeinfo = localtime(&seconds);
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);


   	// Assuming read_serial_data reads sensor data and populates the arrays
        read_serial_file(sensor, speed_magnitudes, range_magnitudes, speeds, ranges);

	if(sensor.is_vehicle_approaching(speeds, speed_magnitudes))
		if(sensor.is_vehicle_in_range(ranges, range_magnitudes)){
       	       		printf("%s.%03d: A vehicle is approaching\n", time_buffer, milliseconds);
		}
    }

    return 0;
}