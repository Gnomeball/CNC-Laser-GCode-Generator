#ifndef gcode_h
#define gcode_h

#define DEBUG_PRINT_OUT

#include <fstream>
#include <vector>

#include "grid.hpp"
#include "line.hpp"
#include "point.hpp"

enum GcodeType { OUTLINE, INFILL };

class Gcode {
    // Fields
    Grid grid;
    std::vector<Point> points;
    std::vector<Line> lines;

    double total_burn_distance;
    double total_travel_distance;

    double efficiency;

    int estimated_time;

    // Utility Functions
    void build_as_outline();
    void build_as_infill();

    void order_lines();

    void simplify_lines();

    void calculate_stats(const int speed, const int travel);

public:
    // Constructor(s) / Destructor(s)
    Gcode() = default;
    Gcode(Grid grid);
    ~Gcode() = default;

    // Member Functions
    double get_burn_distance();
    double get_travel_distance();
    double get_efficiency();
    int get_estimated_time();

    // Utility Functions
    void build(GcodeType type, const int speed, const int travel);
    void write_to_file(std::ofstream &os, const int burn, const int speed, const int travel);

    // Overrides
};

#endif // gcode_h
