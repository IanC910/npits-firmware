#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <iostream>
#include <sstream>
#include <vector>
#include <cstring>

#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "OPS243.h"

OPS243::OPS243(const char serial_port[], int BAUD_RATE) {
    /* Constructor: Mainly initializes the serial port for 
    communication with the OPS243*/

    serial_file = open(serial_port, O_RDWR);

    struct termios tty;

    // Read in existing settings, and handle any error
    if(tcgetattr(serial_file, &tty) != 0) {
        printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
        exit(1);
    }

    tty.c_cflag &= ~PARENB; // Clear parity bit, disabling parity (most common)
    tty.c_cflag &= ~CSTOPB; // Clear stop field, only one stop bit used in communication (most common)
    tty.c_cflag &= ~CSIZE; // Clear all bits that set the data size
    tty.c_cflag |= CS8; // 8 bits per byte (most common)
    tty.c_cflag &= ~CRTSCTS; // Disable RTS/CTS hardware flow control (most common)
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines (CLOCAL = 1)

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed
    // tty.c_oflag &= ~OXTABS; // Prevent conversion of tabs to spaces (NOT PRESENT ON LINUX)
    // tty.c_oflag &= ~ONOEOT; // Prevent removal of C-d chars (0x004) in output (NOT PRESENT ON LINUX)

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    // Set in/out baud rate to be 115200
    cfsetispeed(&tty, BAUD_RATE);
    cfsetospeed(&tty, BAUD_RATE);

    // Save tty settings, also checking for error
    if (tcsetattr(serial_file, TCSANOW, &tty) != 0) {
        printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
        exit(1);
    }
}

OPS243::~OPS243() {
    close(serial_file);
}

void OPS243::output_current_speed_settings() {
    char cmd[] = "O?";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::output_current_range_settings() {
    char cmd[] = "o?";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_speed_output_units() {
    /* Sets the units for which the sensor will use to report speed*/
    /* Hard-coded to m/s */
    char cmd[] = "UM";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_range_output_units() {
    /* Sets the units for which the sensor will use to report range*/
    /* Hard-coded to meters */
    char cmd[] = "UM";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_data_precision(int precision) {
    /* Sets the number of decimal places that will be report */
    if ((precision > 5) || (precision < 0)) {
        printf("Invalid precision value, valid values are between 0 and 5\n");
        return;
    }
    char cmd[32];
    sprintf(cmd, "F%d", precision);
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_minimum_speed_filter(int min_speed) {
    /* Sets the minumum value of speed that will be reported*/
    /* Any number below min_speed will not be reported */
    if (min_speed < 0) {
        printf("Minimum speed must be greater than zero");
        return;
    }

    char cmd[32];
    sprintf(cmd, "R>%d\n", min_speed);
   write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_maximum_speed_filter(int max_speed) {
    /* Sets the maximum value of speed that will be reported*/
    /* Any number above max_speed will not be reported */
    if (max_speed < 0) {
        printf("Maximum speed must be greater than zero");
        return;
    }

    char cmd[32];
    sprintf(cmd, "R<%d\n", max_speed);
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_minimum_range_filter(int min_range) {
    /* Sets the minumum value of range that will be reported*/
    /* Any number below min_range will not be reported */
    if (min_range < 0) {
        printf("Minimum range must be greater than zero");
        return;
    }

    char cmd[32];
    sprintf(cmd, "r>%d\n", min_range);
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_maximum_range_filter(int max_range) {
    /* Sets the maximum value of range that will be reported*/
    /* Any number above max_range will not be reported */
    if (max_range < 0) {
        printf("Maximum range must be greater than zero");
        return;
    }

    char cmd[32];
    sprintf(cmd, "r<%d\n", max_range);
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::report_current_range_filter() {
    char cmd[32] = "r?";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::report_current_speed_filter() {
    char cmd[32] = "R?";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_inbound_only() {
    /* Set the reporting to only be for inbound directions */
    char cmd[32] = "R+\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_outbound_only() {
    /* Sets the reporting to only be for outbound directions */
    char cmd[32] = "R-\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::clear_direction_control() {
    /* Clear direction control */
    char cmd[32] = "R|\n";
    write(serial_file, cmd, sizeof(cmd));
}

/* Speed averaging allows a means of filtering for the peak speed of an object. 
* Some objects due to slight delays in signal path will have multiple speed reports.*/
void OPS243::enable_peak_speed_average() {
    /* Enables speed averaging of peak detected */
    /* values across the nearest two speeds detected.*/
    char cmd[32] = "K+\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::disable_peak_speed_average() {
    /* Disable speed averaging */
    char cmd[32] = "K-\n";
    write(serial_file, cmd, sizeof(cmd));
}

int OPS243::read_buffer(char* read_buf, int length) {
    memset(read_buf, '\0', length);
    int num_bytes = read(serial_file, read_buf, length);
    return num_bytes;
}

void OPS243::clear_buffer() {
    char read_buf[64];
    int num_bytes = 1;
    while(num_bytes != 0) {
        num_bytes = read(serial_file, read_buf, sizeof(read_buf));
    }
}

int OPS243::get_module_info(char* module_info, int length) {
    clear_buffer();

    char cmd[32] = "??";
    write(serial_file, cmd, sizeof(cmd));

    memset(module_info, '\0', length);
    int total_bytes = 0;

    while(length > 0) {
        int num_bytes = read(serial_file, module_info, length);
        if(num_bytes == 0) {
            return total_bytes;
        }
        total_bytes += num_bytes;
        module_info += num_bytes;
        length -= num_bytes;
    }

    return total_bytes;
}

int OPS243::get_serial_file() { return serial_file;}

void read_serial_file(OPS243& obj, int* speed_magnitudes, int* range_magnitudes, float* speeds, float* ranges) {

        char line_buf[256];
        memset(line_buf, 0, sizeof(line_buf));

        int line_buf_index = 0;
        while(line_buf_index < sizeof(line_buf)) {
            int num_bytes_read = read(obj.get_serial_file(), line_buf + line_buf_index, 1);
            if(line_buf[line_buf_index] == '\n') {
                line_buf[line_buf_index] = 0;
                break;
            }

            line_buf_index += num_bytes_read;
        }

        struct timespec now;
        timespec_get(&now, TIME_UTC);
        time_t seconds = now.tv_sec;
        int milliseconds = now.tv_nsec / 1000000;
        struct tm *timeinfo = localtime(&seconds);
        char time_buffer[30];
        strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        //printf("%s.%03d: %s\n", time_buffer, milliseconds, line_buf);

        char *token;
        int count = 0;
        int magnitude_index = 0;
        int speed_index = 0;
        int range_index = 0;
        const char s[2] = ",";
	
        if (line_buf[1] == 'm' && line_buf[3] != 's') {
            token = strtok(line_buf, s);

            while (token) {
                //printf("%s\n", token); 
                if (count == 0) {
                    token = strtok(NULL, s); // Ignore the "m"
                    count++;
                    continue;
                }
                if ((count % 2) != 0) {
                    range_magnitudes[magnitude_index] = atoi(token);
                    magnitude_index++;
                }
                if ((count % 2) == 0 && count != 0) {
                    ranges[range_index] = atof(token);
                    range_index++;
                }
                count++;
                token = strtok(NULL, s);
            }/*
 	    printf("Magnitudes: ");
            for(int i = 0; i < magnitude_index; i++) {
                printf("%d,", range_magnitudes[i]);
            }

            printf("Ranges:");
            for(int j = 0; j < range_index; j++) {
                printf("%f,", ranges[j]);
            }
	    printf("\n"); */
            return;
        }
        
        if (line_buf[3] == 's') {
            token = strtok(line_buf, s);

            while (token) {
                if (count == 0) {
                    token = strtok(NULL, s); // Ignore the "mps"
                    count++;
                    continue;
                }
                if ((count % 2) != 0) {
                    speed_magnitudes[magnitude_index] = atoi(token);
                    magnitude_index++;
                }
                if ((count % 2) == 0 && count != 0) {
                    speeds[range_index] = atof(token);
                    speed_index++;
                }
                count++;
                token = strtok(NULL, s);
            }
        	/*

            printf("Magnitudes: ");
            for(int i = 0; i < magnitude_index; i++) { 
                printf("%d,", speed_magnitudes[i]);
            }

            printf("Speeds:"); 
            for(int j = 0; j < speed_index; j++) {
                printf("%f,", speeds[j]);
            }
	    printf("\n"); */
	return;
	}
}

void OPS243::turn_range_reporting_on() {
    char cmd[32] = "OD";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_range_reporting_off() {
    char cmd[32] = "Od";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_speed_reporting_on() {
    char cmd[32] = "OS";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_speed_reporting_off() {
    char cmd[32] = "Os";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_fmcw_magnitude_reporting_on() {
    char cmd[32] = "oM";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_fmcw_magnitude_reporting_off() {
    char cmd[32] = "om";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_doppler_magnitude_reporting_on() {
    char cmd[32] = "OM";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_doppler_magnitude_reporting_off() {
    char cmd[32] = "Om";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_largest_report_order_on() {
     char cmd[] = "OV";
     write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_number_of_speed_reports(int number_of_reports) {
     if (number_of_reports > 9 || number_of_reports < 1) {
	     printf("Invalid number of reports set\n");
	     return;
     }
     char cmd[32];
     sprintf(cmd, "O%d", number_of_reports);
     write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_number_of_range_reports(int number_of_reports) {
     if (number_of_reports > 9 || number_of_reports < 1) {
	     printf("Invalid number of reports set\n");
	     return;
     }
     char cmd[32];
     sprintf(cmd, "o%d", number_of_reports);
     write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_binary_output_on() {
    char cmd[32] = "OB";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_binary_output_off() {
    char cmd[32] = "Ob";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_JSON_output_on() {
    char cmd[32] = "OJ";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_JSON_output_off() {
    char cmd[32] = "Oj";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_units_output_on() {
    char cmd[32] = "OU";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::turn_units_output_off() {
    char cmd[32] = "ou";
    write(serial_file, cmd, sizeof(cmd));
}
