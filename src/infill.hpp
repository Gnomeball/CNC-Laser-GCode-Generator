#ifndef infill_h
#define infill_h

#include <fstream>
#include <iostream>
#include <vector>

#include "misc/debug.hpp"
#include "misc/stats.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"
#include "types/point.hpp"

class Infill {

    private:

        Grid grid;

        Point start;

        std::vector<Line> lines;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

    public:

        // Constructors

        Infill() {} // default

        Infill(Grid grid, Point start, int laser_power, int burn_speed, int travel_speed)
        : grid(grid), start(start), laser_power(laser_power), burn_speed(burn_speed), travel_speed(travel_speed) {
            this->lines = std::vector<Line>();
        }

        // Accessors

        Stats get_stats() {
            return this->stats;
        }

        // Helpers

        void construct_lines(const int density) {

            // Build lines

            Point p_start;
            Point p_end;

            int line_skip = 10 / density;

            for (int y = 1; y < this->grid.get_height() - 1; y += line_skip) {
                for (int x = 1; x < this->grid.get_width() - 1; x++) {
                    if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x - 1, y)) {
                        // this is the start of a line
                        p_start = Point(x, y);
                    }
                    if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x + 1, y)) {
                        // this is the end of a line
                        p_end = Point(x, y);
                        Line temp = Line(p_start, p_end);
                        if (temp.length() != 0) { // check for zero-length lines and discard them
                            this->lines.push_back(temp);
                        }
                    }
                }
            }

#ifdef DEBUG_INFILL
            std::cout << "Constructed Infill : " << lines.size() << " lines" << std::endl;
#endif

            // Order lines

            std::vector<Line> temp = order_lines(this->lines, this->start);

            this->lines = temp;

#ifdef DEBUG_INFILL
            std::cout << "Ordered Infill" << std::endl;
#endif
        }

        void calculate_stats() {
            this->stats = Stats(lines, this->burn_speed, this->travel_speed);
#ifdef DEBUG_INFILL
            std::cout << "Infill Stats Calculated" << std::endl;
#endif
        }

        void write_to_file(std::ofstream &os) {
            for (int l = 0; l < (int)this->lines.size(); l++) {
                os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
            }
        }

        // Overrides
};

#endif // infill_h
