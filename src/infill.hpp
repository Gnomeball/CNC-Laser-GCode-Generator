#ifndef infill_h
#define infill_h

#include <fstream>
#include <vector>

#include "grid.hpp"
#include "line.hpp"
#include "stats.hpp"

class Infill {
    // Fields
    Grid grid;
    std::vector<Line> lines;

    Stats stats;

    int burn_speed;
    int laser_power;
    int travel_speed;

public:
    // Constructor(s) / Destructor(s)
    Infill() = default;
    Infill(Grid grid, int burn, int laser, int travel);
    ~Infill() = default;

    // Member Functions
    Stats get_stats();

    // Utility Functions
    void construct_lines();
    void calculate_stats();
    void write_to_file(std::ofstream &os);

    // Overrides
};

#endif // infill_h
