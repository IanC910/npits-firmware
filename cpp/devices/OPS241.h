
#ifndef OPS241_H
#define OPS241_H

#define OPS241_BAUD_RATE B115200

class OPS241 {
public:
    static const int REPORT_RATE_HZ = 14;

    OPS241(const char serial_port[]);
    ~OPS241();

    int read_buffer(char* read_buf, int length);
    void clear_buffer();

    // Puts at most length bytes of the module info string into module_info
    // Returns number of bytes read
    int get_module_info(char* module_info, int length);

    void start_reporting_distance();
    void stop_reporting_distance();

    void set_num_digits(unsigned int num_digits);

    float read_distance_m();

private:
    int serial_file;
};

#endif