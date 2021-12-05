#ifndef stats_h
#define stats_h

#include <vector>

#include "line.hpp"

class Stats {
    // Fields
    double burn_distance;
    double travel_distance;
    double efficiency;
    int time;

    void calculate(std::vector<Line> lines, double speed, double travel);

public:
    // Constructor(s) / Destructor(s)
    Stats() = default;
    Stats(std::vector<Line> lines, double speed, double travel);
    ~Stats() = default;

    // Member Functions
    double get_burn_distance();
    double get_travel_distance();
    double get_efficiency();
    int get_time();

    // Utility Functions

    // Overrides
};

#endif // stats_h
