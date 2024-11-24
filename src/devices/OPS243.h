
#ifndef OPS243_H
#define OPS243_H


class OPS243 {
public:
    static const int REPORT_RATE_HZ = 14;

    OPS243(const char serial_port[], int BAUD_RATE);
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
    void set_maximum_speed_filter(int max_speed);
    void set_minimum_range_filter(int min_range);
    void set_maximum_range_filter(int max_range);

    void report_current_range_filter();
    void report_current_speed_filter();

    void set_inbound_only();
    void set_outbound_only();
    void clear_direction_control();

    void enable_peak_speed_average();
    void disable_peak_speed_average();

    void turn_distance_reporting_on();
    void turn_distance_reporting_off();
    
    void turn_speed_reporting_on();
    void turn_speed_reporting_off();

    int get_serial_file();
    void print_serial_file(OPS243& obj);

    void turn_magnitude_reporting_on();
    void turn_magnitude_reporting_off();

    void turn_largest_report_order_on();
    void set_number_of_reports(int number_of_reports);

private:
    int serial_file;
};

#endif
