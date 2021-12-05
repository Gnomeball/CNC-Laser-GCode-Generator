#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "debug.hpp"
#include "grid.hpp"
#include "infill.hpp"
#include "line.hpp"
#include "point.hpp"
#include "stats.hpp"

Infill::Infill(Grid grid, int burn, int laser, int travel)
    : grid(grid), burn_speed(burn), laser_power(laser), travel_speed(travel) {
    this->lines = std::vector<Line>();
}

void Infill::construct_lines() {

    // * Build lines

    Point p_start;
    Point p_end;

    for (int y = 1; y < this->grid.get_height() - 1; y++) {
        for (int x = 1; x < this->grid.get_width() - 1; x++) {
            if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x - 1, y)) {
                // this is the start of a line
                p_start = Point(x, y);
            }
            if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x + 1, y)) {
                // this is the end of a line
                p_end = Point(x, y);
                this->lines.push_back(Line(p_start, p_end));
            }
        }
    }

#ifdef DEBUG_PRINT_OUT
    std::cout << "Constructed Infill Vector : " << lines.size() << " lines" << std::endl;
#endif

    // * Order lines

    std::vector<Line> temp = std::vector<Line>();

    double start;
    double end;
    double distance_from_last;
    double minimum_travel = INFINITY;

    Line closest;
    int closest_index;

    temp.push_back(Line(Point(0, 0), Point(0, 0)));

    while (this->lines.size() > 0) {

        int index = 0;
        for (Line l : this->lines) {

            start = Line(temp.back().get_end(), l.get_start()).length();
            end = Line(temp.back().get_end(), l.get_end()).length();

            if (end < start)
                l.swap_points();

            distance_from_last = std::min(start, end);

            if (distance_from_last <= minimum_travel) {
                closest = l;
                closest_index = index;
                minimum_travel = distance_from_last;
            }
            index += 1;
        }

        temp.push_back(closest);
        this->lines.erase(this->lines.begin() + closest_index);

        minimum_travel = INFINITY;
    }

    temp.erase(temp.begin());
    this->lines = temp;

#ifdef DEBUG_PRINT_OUT
    std::cout << "Ordered Infill Vector" << std::endl;
#endif
}

void Infill::calculate_stats() {
    this->stats = Stats(lines, this->burn_speed, this->travel_speed);
#ifdef DEBUG_PRINT_OUT
    std::cout << "Infill Stats Calculated" << std::endl;
#endif
}

Stats Infill::get_stats() {
    return this->stats;
}

void Infill::write_to_file(std::ofstream &os) {
    for (int l = 0; l < (int)this->lines.size(); l++)
        os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
}
