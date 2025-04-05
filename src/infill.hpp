#ifndef infill_h
#define infill_h

#include <fstream>
#include <vector>

#include "misc/stats.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"

class Infill {

    private:

        Grid grid;

        std::vector<Line> lines;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

    public:

        // Constructors

        Infill() {} // default

        Infill(Grid grid, int laser_power, int burn_speed, int travel_speed)
        : grid(grid), laser_power(laser_power), burn_speed(burn_speed), travel_speed(travel_speed) {
            this->lines = std::vector<Line>();
        }

        // Accessors

        Stats get_stats() {
            return this->stats;
        }

        // Helpers

        void construct_lines() {

            // Build lines

            Point p_start;
            Point p_end;

            // todo; change needed on y value for density
            for (int y = 1; y < this->grid.get_height() - 1; y += 2) {
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

            // Order lines

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

        void calculate_stats() {
            this->stats = Stats(lines, this->burn_speed, this->travel_speed);
        #ifdef DEBUG_PRINT_OUT
            std::cout << "Infill Stats Calculated" << std::endl;
        #endif
        }

        void write_to_file(std::ofstream &os) {
            for (int l = 0; l < (int)this->lines.size(); l++)
                os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
        }

        // Overrides
};

#endif // infill_h
