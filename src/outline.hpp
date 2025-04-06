#ifndef outline_h
#define outline_h

#include <fstream>
#include <iostream>
#include <vector>

#include "misc/debug.hpp"
#include "misc/stats.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"
#include "types/point.hpp"

class Outline {

    private:

        Grid grid;

        std::vector<Line> lines;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

    public:

        // Constructors

        Outline() {} // default

        Outline(Grid grid, int laser_power, int burn_speed, int travel_speed)
        : grid(grid), laser_power(laser_power), burn_speed(burn_speed), travel_speed(travel_speed) {
            this->lines = std::vector<Line>();
        }

        // Accessors

        Stats get_stats() {
            return this->stats;
        }

        // Helpers

        void construct_lines() {

            // Collect all points that exist on the outline

            std::vector<Point> points = std::vector<Point>();

            for (int y = 1; y < this->grid.get_height() - 1; y++) {
                for (int x = 1; x < this->grid.get_width() - 1; x++) {
                    if (this->grid.get_pixel(x, y)) {
                        // this is an edge pixel
                        points.push_back(Point(x, y));
                    }
                }
            }

            // Build all possible lines of length < 2 between all points

            std::vector<Line> lines = std::vector<Line>();

            for (int left_index = 0; left_index < (int)points.size(); left_index++) {
                for (int right_index = left_index; right_index < (int)points.size(); right_index++) {

                    Point start = points.at(left_index);
                    Point end = points.at(right_index);

                    Line temp = Line(start, end);
                    if (start != end && temp.length() < 2) {
                        // < 2 because a diagonal line will be sqrt(2) in length
                        lines.push_back(temp);
                    }
                }
            }

            this->lines = lines;

#ifdef DEBUG_OUTLINE
            std::cout << "Constructed Outline : " << lines.size() << " lines" << std::endl;
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

#ifdef DEBUG_OUTLINE
            std::cout << "Ordered Outline" << std::endl;
#endif

            for (int l = 1; l < (int)this->lines.size(); l++) {
                Line current = this->lines.at(l);
                Line previous = this->lines.at(l-1);
                if (previous.get_end() == current.get_start()) {
                    this->lines.at(l).set_continuation(true);
                }
            }

#ifdef DEBUG_OUTLINE
            std::cout << "Outline Continuation Configured" << std::endl;
#endif
        }

        void calculate_stats() {
            this->stats = Stats(lines, this->burn_speed, this->travel_speed);
#ifdef DEBUG_OUTLINE
            std::cout << "Outline Stats Calculated" << std::endl;
#endif
        }

        void write_to_file(std::ofstream &os) {
            for (int l = 0; l < (int)this->lines.size(); l++) {
                os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
            }
        }

        // Overrides
};

#endif // outline_h
