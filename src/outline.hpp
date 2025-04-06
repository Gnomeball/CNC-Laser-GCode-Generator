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

        Point finish;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

        inline bool line_already_exists(std::vector<Line> lines, Line line) {
            Line reverse = Line(line.get_end(), line.get_start());
            for (Line l : lines) {
                if (l == line || l == reverse) {
                    return true;
                }
            }
            return false;
        }

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

        Point get_finish() {
            return this->finish;
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

            // Find outline edges on cardinal directions

            std::vector<Line> cardinals = std::vector<Line>();

            for (Point p : points) {
                int this_x = p.get_x();
                int this_y = p.get_y();

                Point u = Point(this_x, this_y - 1);
                Point r = Point(this_x + 1, this_y);
                Point d = Point(this_x, this_y + 1);
                Point l = Point(this_x - 1, this_y);

                for (Point q : { u, r, d, l }) {
                    if (this->grid.get_pixel(q.get_x(), q.get_y())) {
                        // if Q is also an edge
                        if (!line_already_exists(cardinals, Line(q, p))) {
                            cardinals.push_back(Line(p, q));
                        }
                    }
                }
            }

            // Find outline edges on diagonal directions

            std::vector<Line> diagonals = std::vector<Line>();

            for (Point p : points) {
                int this_x = p.get_x();
                int this_y = p.get_y();

                Point ur = Point(this_x + 1, this_y - 1);
                Point dr = Point(this_x + 1, this_y + 1);
                Point dl = Point(this_x - 1, this_y + 1);
                Point ul = Point(this_x - 1, this_y - 1);

                for (Point q : { ur, dr, dl, ul }) {
                    if (this->grid.get_pixel(q.get_x(), q.get_y())) {
                        // if Q is also an edge
                        if (!line_already_exists(diagonals, Line(q, p))) {
                            diagonals.push_back(Line(p, q));
                        }
                    }
                }
            }

            // Remove unecessary diagonal lines (favouring cardinal ones)

            std::vector<Line> pruned_diagonals = std::vector<Line>();

            for (Line diagonal : diagonals) {
                // std::cout << diagonal.angle() << ", "; // \ = -45, \ == 45

                Point a = diagonal.get_start();
                Point b = diagonal.get_end();

                Point c = Point(a.get_x(), b.get_y());
                Point d = Point(b.get_x(), a.get_y());

                Line a_c = Line(a, c);
                Line a_d = Line(a, d);

                Line b_c = Line(b, c);
                Line b_d = Line(b, d);

                if (!(line_already_exists(cardinals, a_d) && line_already_exists(cardinals, b_d)) &&
                    !(line_already_exists(cardinals, a_c) && line_already_exists(cardinals, b_c))) {
                    pruned_diagonals.push_back(diagonal);
                }
            }

            // Concatenate the lists

            for (Line l : pruned_diagonals) {
                cardinals.push_back(l);
            }

            this->lines = cardinals;

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

            this->finish = this->lines.back().get_end();

#ifdef DEBUG_OUTLINE
            std::cout << "Ordered Outline" << std::endl;
#endif

            for (int l = 1; l < (int)this->lines.size(); l++) {
                Line current = this->lines.at(l);
                Line previous = this->lines.at(l - 1);
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
