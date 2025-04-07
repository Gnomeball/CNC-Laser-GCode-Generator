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

        std::vector<Point> edge;

        std::vector<Line> lines;

        Point finish;

        Stats stats;

        int laser_power;

        int burn_speed;
        int travel_speed;

        // Helpers

        bool point_in_edge(Point point) {
            for (Point p : this->edge) {
                if (p == point) {
                    return true;
                }
            }
            return false;
        }

        bool line_already_exists(std::vector<Line> lines, Line line) {
            Line reverse = Line(line.get_end(), line.get_start());
            for (Line l : lines) {
                if (l == line || l == reverse) {
                    return true;
                }
            }
            return false;
        }

        // Process Methods

        void build_edge_points() {
            for (int y = 1; y < this->grid.get_height() - 1; y++) {
                for (int x = 1; x < this->grid.get_width() - 1; x++) {
                    if (this->grid.get_pixel(x, y)) {
                        // this is an edge pixel
                        this->edge.push_back(Point(x, y));
                    }
                }
            }
        }

        std::vector<Line> build_edge_part(std::vector<std::vector<int>> part) {
            std::vector<Line> lines = std::vector<Line>();

            for (Point p : this->edge) {
                int this_x = p.get_x();
                int this_y = p.get_y();

                std::vector<Point> points = {
                    Point(this_x + part[0][0], this_y + part[0][1]),
                    Point(this_x + part[1][0], this_y + part[1][1]),
                    Point(this_x + part[2][0], this_y + part[2][1]),
                    Point(this_x + part[3][0], this_y + part[3][1])
                };

                for (Point q : points) {
                    // if Q is also an edge point
                    if (point_in_edge(q)) {
                        Line possible = Line(q, p);
                        // and the same line, but in the other direction, doesn't already exist
                        if (!line_already_exists(lines, possible)) {
                            // swap their points, and add the line
                            possible.swap_points();
                            lines.push_back(possible);
                        }
                    }
                }
            }

            return lines;
        }

    public:

        // Constructors

        Outline() {} // default

        Outline(Grid grid, int laser_power, int burn_speed, int travel_speed)
        : grid(grid), laser_power(laser_power), burn_speed(burn_speed), travel_speed(travel_speed) {
            this->lines = std::vector<Line>();
        }

        // Accessors

        Point get_finish() {
            return this->finish;
        }

        // Helpers

        void construct_lines() {

#ifdef DEBUG_OUTLINE
            std::cout << "Constructing Outline : " << std::endl;
#endif

            // Collect all points that exist on the outline

            build_edge_points();

#ifdef DEBUG_OUTLINE
            std::cout << "  Found Outline Points : " << this->edge.size() << " points" << std::endl;
#endif

            // Find outline edges on cardinal directions

            std::vector<std::vector<int>> card_points = {
                { 0, -1 }, { 1, 0 },
                { 0, 1 }, { -1, 0 }
            };

            std::vector<Line> cardinals = build_edge_part(card_points);

            // Find outline edges on diagonal directions

            std::vector<std::vector<int>> diag_points = {
                { 1, -1 }, { 1, 1 },
                { -1, 1 }, { -1, -1 }
            };

            std::vector<Line> diagonals = build_edge_part(diag_points);

#ifdef DEBUG_OUTLINE
            std::cout << "  Generated Outline Parts : " << cardinals.size() << " + " << diagonals.size() << " lines" << std::endl;
#endif

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

#ifdef DEBUG_OUTLINE
            std::cout << "  Removed Unnecessary Lines : " << diagonals.size() - pruned_diagonals.size() << " lines removed" << std::endl;
#endif

            // Concatenate the lists

            for (Line l : pruned_diagonals) {
                cardinals.push_back(l);
            }

            this->lines = cardinals;

#ifdef DEBUG_OUTLINE
            std::cout << "  Found Outline : " << lines.size() << " lines" << std::endl;
#endif

            // Order lines

            std::vector<Line> temp = order_lines(this->lines, Point(0, 0));

            this->lines = temp;

            // Set finishing point, so Infill can continue from there rather than (0, 0)

            this->finish = this->lines.back().get_end();

#ifdef DEBUG_OUTLINE
            std::cout << "  Ordered Outline" << std::endl;
#endif

            for (int l = 1; l < (int)this->lines.size(); l++) {
                Line current = this->lines.at(l);
                Line previous = this->lines.at(l - 1);
                if (previous.get_end() == current.get_start()) {
                    this->lines.at(l).set_continuation(true);
                }
            }

#ifdef DEBUG_OUTLINE
            std::cout << "  Outline Continuation Configured" << std::endl;
#endif

            this->stats = Stats(this->lines, this->burn_speed, this->travel_speed);

#ifdef DEBUG_OUTLINE
            std::cout << "  Outline Stats Calculated" << std::endl;
#endif
        }

        void print_stats(std::ofstream &os) {
            os << this->stats.to_string("Outline");
        }

        void write_to_file(std::ofstream &os) {
            for (int l = 0; l < (int)this->lines.size(); l++) {
                os << this->lines.at(l).to_string(this->laser_power, this->burn_speed, this->travel_speed);
            }
        }

        // Overrides
};

#endif // outline_h
