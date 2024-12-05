#ifndef OPS243_H
#define OPS243_H

#include <string>

class OPS243 {
public:
    static const int REPORT_RATE_HZ = 14;
    static const int SERIAL_BAUD_RATE = 115200;
    static const int MAX_REPORTS = 9;

    struct speed_report_t {
        float speed_mps;
        int magnitude;
    };

    struct range_report_t {
        float range_m;
        int magnitude;
    };

    OPS243(const std::string serial_port);
    ~OPS243();

    bool connect_to_port(const std::string serial_port);
    bool is_connected();

    int get_module_info(char* module_info, int length);
    int read_buffer(char* read_buf, int length);
    void clear_buffer();

    void output_current_speed_settings();
    void output_current_range_settings();

    void set_speed_units_to_mps();
    void set_range_units_to_m();

    void set_data_precision(int precision);

    void set_min_speed_mps(int min_speed_mps);
    void set_max_speed_mps(int max_speed_mps);
    void set_min_range_m(int min_range_m);
    void set_max_range_m(int max_range_m);

    void set_min_speed_magnitude(int min_speed_magnitude);
    void set_max_speed_magnitude(int max_speed_magnitude);
    void set_min_range_magnitude(int min_range_magnitude);
    void set_max_range_magnitude(int max_range_magnitude);

    void report_current_range_filter();
    void report_current_speed_filter();

    void set_inbound_only();
    void set_outbound_only();
    void clear_direction_control();


    /* Speed averaging allows a means of filtering for the peak speed of an object.
    * Some objects due to slight delays in signal path will have multiple speed reports.*/
    void enable_peak_speed_average();
    void disable_peak_speed_average();

    void turn_range_reporting_on();
    void turn_range_reporting_off();

    void turn_speed_reporting_on();
    void turn_speed_reporting_off();

    // Reads the new line and updates either the range_reports OR the speed_reports, not both
    // Returns 1 if the ranges were updated
    // Returns 2 if the speeds were updated
    // Returns 0 if neither
    // Blocking
    int read_new_data_line(range_report_t* range_reports, speed_report_t* speed_reports);

    void turn_range_magnitude_reporting_on();
    void turn_range_magnitude_reporting_off();

    void turn_speed_magnitude_reporting_on();
    void turn_speed_magnitude_reporting_off();

    void turn_largest_report_order_on();
    void set_num_range_reports(int number_of_reports);
    void set_num_speed_reports(int number_of_reports);

    void turn_binary_output_on();
    void turn_binary_output_off();

    void turn_JSON_output_on();
    void turn_JSON_output_off();

    void turn_units_output_on();
    void turn_units_output_off();
private:
    bool is_connected_local = false;
    int serial_file;
};

#endif