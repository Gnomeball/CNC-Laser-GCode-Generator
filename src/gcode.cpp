#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <vector>

#include "gcode.hpp"
#include "grid.hpp"
#include "line.hpp"
#include "point.hpp"

Gcode::Gcode(Grid grid) : grid(grid) {
    this->points = std::vector<Point>();
    this->lines = std::vector<Line>();
}

double Gcode::get_burn_distance() {
    return this->total_burn_distance;
}

double Gcode::get_travel_distance() {
    return this->total_travel_distance;
}

double Gcode::get_efficiency() {
    return this->efficiency;
}

int Gcode::get_estimated_time() {
    return this->estimated_time;
}

void Gcode::build_as_outline() {

    // * Build points
    for (int y = 0; y < this->grid.get_height(); y++) {
        for (int x = 0; x < this->grid.get_width(); x++) {
            if (this->grid.get_pixel(x, y) == 1) {
                this->points.push_back(Point(x, y));
            }
        }
    }

#ifdef DEBUG_PRINT_OUT
    std::cout << "Made outline vector of " << this->points.size() << " points" << std::endl;
#endif

    // * Build lines
    for (int a = 0; a < (int)this->points.size(); a++) {
        for (int b = a; b < (int)this->points.size(); b++) {
            Line temp(this->points.at(a), this->points.at(b));
            if (temp.length() == 0)
                continue;
            else if (temp.length() < 2)
                this->lines.push_back(temp);
        }
    }

#ifdef DEBUG_PRINT_OUT
    std::cout << "Made outline vector of " << this->lines.size() << " lines" << std::endl;
#endif

    // * Order lines
    this->order_lines();

    // * Simplify lines (the scary one)
    this->simplify_lines();
}

void Gcode::build_as_infill() {

    Point start;
    Point end;

    // * Build points and lines
    for (int y = 1; y < this->grid.get_height() - 1; y++) {
        for (int x = 1; x < this->grid.get_width() - 1; x++) {
            if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x - 1, y)) {
                // this is the start of a line
                start = Point(x, y);
                this->points.push_back(start);
            }
            if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x + 1, y)) {
                // this is the end of a line
                end = Point(x, y);
                this->points.push_back(end);
                this->lines.push_back(Line(start, end));
            }
        }
    }

#ifdef DEBUG_PRINT_OUT
    std::cout << "Made infill vector of " << this->points.size() << " points" << std::endl;
    std::cout << "Made infill vector of " << this->lines.size() << " lines" << std::endl;
#endif

    // * Order lines
    this->order_lines();
}

void Gcode::order_lines() {

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
    std::cout << "Ordered vector of " << this->lines.size() << " lines" << std::endl;
#endif
}

void Gcode::simplify_lines() {

    std::vector<Line> temp = std::vector<Line>();

    Line current = this->lines[0];
    Line temp_line;

    temp.push_back(current);
    this->lines.erase(this->lines.begin());

    while (this->lines.size() > 0) {
        if (this->lines[0].angle() == current.angle() && this->lines[0].get_start() == current.get_end()) {

            temp_line = current;

            do {
                temp_line = Line(temp_line.get_start(), this->lines[0].get_end());
                this->lines.erase(this->lines.begin());
            } while (temp_line.angle() == this->lines[0].angle());

            temp.push_back(temp_line);

            current = this->lines[0];
            this->lines.erase(this->lines.begin());

        } else {

            temp.push_back(current);

            current = this->lines[0];
            this->lines.erase(this->lines.begin());

        }
    }

    this->lines = temp;
#ifdef DEBUG_PRINT_OUT
    std::cout << "Simplified vector of " << this->lines.size() << " lines" << std::endl;
#endif
}

// void Gcode::calculate_stats(const int speed, const int travel) {

//     // * Reset
//     this->total_burn_distance = 0;
//     this->total_travel_distance = 0;

//     Point last = Point(0, 0);

//     // * Distances
//     for (Line l : this->lines) {
//         // burn
//         this->total_burn_distance += (l.length() / 10);
//         // travel
//         Line travel(last, l.get_start());
//         this->total_travel_distance += (travel.length() / 10);
//         last = l.get_end();
//     }

//     // * Efficiency
//     this->efficiency = (double)this->total_burn_distance /
//                        (this->total_burn_distance + this->total_travel_distance) * 100;

//     // * Time
//     this->estimated_time =
//         (this->total_burn_distance / speed * 10) + (this->total_travel_distance / travel * 10);
// }

// void Gcode::build(GcodeType type, const int speed, const int travel) {
//     switch (type) {
//         case OUTLINE:
//             this->build_as_outline();
//             break;
//         // case INFILL:
//         //     this->build_as_infill();
//         //     break;
//         default:; // Unreachable
//     }
//     this->calculate_stats(speed, travel);
// }

// void Gcode::write_to_file(std::ofstream &os, const int burn, const int speed, const int travel) {
//     for (int l = 0; l < (int)this->lines.size(); l++) {
//         os << this->lines.at(l).to_string(burn, speed, travel);
//     }
// }
