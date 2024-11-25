#ifndef OPS243_H
#define OPS243_H


class OPS243 {
public:
    static const int REPORT_RATE_HZ = 14;

    OPS243(const char serial_port[], int BAUD_RATE);
    ~OPS243();

    int get_module_info(char* module_info, int length);
    int read_buffer(char* read_buf, int length);
    void clear_buffer();

    void output_current_speed_settings();
    void output_current_range_settings();

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

    void turn_range_reporting_on();
    void turn_range_reporting_off();
    
    void turn_speed_reporting_on();
    void turn_speed_reporting_off();

    int get_serial_file();
    void read_serial_file(OPS243& obj, int* speed_magnitudes, int* range_magnitudes, float* speeds, float* ranges);

    void turn_fmcw_magnitude_reporting_on();
    void turn_fmcw_magnitude_reporting_off();

    void turn_doppler_magnitude_reporting_on();
    void turn_doppler_magnitude_reporting_off();

    void turn_largest_report_order_on();
    void set_number_of_range_reports(int number_of_reports);
    void set_number_of_speed_reports(int number_of_reports);

    void turn_binary_output_on();
    void turn_binary_output_off();
   
    void turn_JSON_output_on();
    void turn_JSON_output_off();

    void turn_units_output_on();
    void turn_units_output_off(); 
private:
    int serial_file;
};

#endif


