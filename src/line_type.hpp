#ifndef line_type_h
#define line_type_h

#include <fstream>
#include <vector>

#include "misc/stats.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"
#include "types/point.hpp"

class LineType {

    protected:

        Grid grid;

        Point start;

        std::vector<Line> lines;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

        // Helpers

        bool line_already_exists(const std::vector<Line> lines, const Line line) const {
            Line reverse = Line(line.get_end(), line.get_start());
            for (Line l : lines) {
                if (l == line || l == reverse) {
                    return true;
                }
            }
            return false;
        }

        std::vector<Line> order_lines() {

            std::vector<Line> temp = std::vector<Line>();

            double start;
            double end;
            double distance_from_last;
            double minimum_travel = INFINITY;

            Line closest;
            int closest_index;

            temp.push_back(Line(Point(0, 0), this->start));

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

            return temp;
        }

        void generate_stats() {
            this->stats = Stats(this->lines, this->burn_speed, this->travel_speed);
        }

    public:

        // Constructors

        LineType() {} // default

        LineType(const Grid grid, const Point start, const int laser_power, const int burn_speed, const int travel_speed)
        : grid(grid), start(start), laser_power(laser_power), burn_speed(burn_speed), travel_speed(travel_speed) {
            this->lines = std::vector<Line>();
        }

        // Accessors

        // Helpers

        void print_stats(std::ofstream &os) {
            os << this->stats.to_string();
        }

        void write_to_file(std::ofstream &os) {
            for (int l = 0; l < (int)this->lines.size(); l++) {
                os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
            }
        }

        // Overrides
};

#endif // oline_type_h
