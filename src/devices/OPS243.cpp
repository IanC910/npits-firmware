
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>

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

void OPS243::set_speed_output_units() {
    /* Sets the units for which the sensor will use to report speed*/
    /* Hard-coded to km/h */
    char cmd[] = "UK";
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
    char cmd[3];
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

    char cmd[3];
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

    char cmd[3];
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

    char cmd[3];
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

    char cmd[3];
    sprintf(cmd, "r<%d\n", max_range);
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_inbound_only() {
    /* Set the reporting to only be for inbound directions */
    char cmd[] = "R+\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::set_outbound_only() {
    /* Sets the reporting to only be for outbound directions */
    char cmd[] = "R-\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::clear_direction_control() {
    /* Clear direction control */
    char cmd[] = "R|\n";
    write(serial_file, cmd, sizeof(cmd));
}

/* Speed averaging allows a means of filtering for the peak speed of an object. 
/* Some objects due to slight delays in signal path will have multiple speed reports.*/
void OPS243::enable_peak_speed_average() {
    /* Enables speed averaging of peak detected */
    /* values across the nearest two speeds detected.*/
    char cmd[] = "K+\n";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::disable_peak_speed_average() {
    /* Disable speed averaging */
    char cmd[] = "K-\n";
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

    char cmd[] = "??";
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

void OPS243::start_reporting_distance() {
    char cmd[] = "oD";
    write(serial_file, cmd, sizeof(cmd));
}

void OPS243::stop_reporting_distance() {
    char cmd[] = "od";
    write(serial_file, cmd, sizeof(cmd));
}

