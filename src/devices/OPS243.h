
#ifndef OPS243_H
#define OPS243_H

#define OPS243_BAUD_RATE B115200

class OPS243 {
public:
    static const int REPORT_RATE_HZ = 14;

    OPS243(const char serial_port[]);
    ~OPS243();

    int read_buffer(char* read_buf, int length);
    void clear_buffer();

    // Puts at most length bytes of the module info string into module_info
    // Returns number of bytes read
    int get_module_info(char* module_info, int length);
    void set_speed_output_units();
    void set_range_output_units();

    void set_data_precision(int precision);
    void set_minimum_speed_filter(int min_speed);
    void set_maxmimum_speed_filter(int max_speed);
    void set_minimum_range_filter(int min_range);
    void set_maximum_range_filter(int max_range);

    void set_inbound_only();
    void set_outbound_only();
    void clear_direction_control();

    void enable_peak_speed_average();
    void disable_peak_speed_average();
    
    void start_reporting_distance();
    void stop_reporting_distance();

    void start_reporting_speed();
    void stop_reporting_speed();


    void set_num_digits(unsigned int num_digits);

private:
    int serial_file;
};

#endif