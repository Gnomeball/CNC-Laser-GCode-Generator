#ifndef outline_h
#define outline_h

#include <iostream>
#include <vector>

#include "line_type.hpp"

#include "misc/debug.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"
#include "types/point.hpp"

class Outline : public LineType {

    private:

        std::vector<Point> edge;

        std::vector<Line> cardinals;
        std::vector<Line> diagonals;

        Point finish;

        // Helpers

        bool point_in_edge(const Point point) const {
            for (Point p : this->edge) {
                if (p == point) {
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

        std::vector<Line> build_edge_part(const std::vector<std::vector<int>> part) {
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

        std::vector<Line> prune_diagonals() {
            std::vector<Line> pruned_diagonals = std::vector<Line>();

            for (Line diagonal : this->diagonals) {
                // std::cout << diagonal.angle() << ", "; // \ = -45, \ == 45

                Point a = diagonal.get_start();
                Point b = diagonal.get_end();

                Point c = Point(a.get_x(), b.get_y());
                Point d = Point(b.get_x(), a.get_y());

                Line a_c = Line(a, c);
                Line a_d = Line(a, d);

                Line b_c = Line(b, c);
                Line b_d = Line(b, d);

                if (!(line_already_exists(this->cardinals, a_d) && line_already_exists(this->cardinals, b_d)) &&
                !(line_already_exists(this->cardinals, a_c) && line_already_exists(this->cardinals, b_c))) {
                    pruned_diagonals.push_back(diagonal);
                }
            }

            return pruned_diagonals;
        }

        std::vector<Line> clean_outline() {
            /*
                Make new list, and add first line from current lines
                Pop the first entry from the current lines array
                For line in lines,
                if it has the same angle as the one as the back of the new array,
                    replace the end of that one with the end of this one,
                    otherwise we add it to the new list
                At the end of the array you have a new array with cleaned lines
            */

            std::vector<Line> cleaned_outline = std::vector<Line>();

            // Push the first line onto our new array, then remove it from the global array
            cleaned_outline.push_back(this->lines.front());
            this->lines.erase(this->lines.begin());

            // For the remaining lines, if the angle matches, append it to the previous, otherwise add it
            for (Line line : this->lines) {
                // If angle matches
                if (line.angle() == cleaned_outline.back().angle()) {
                    // Check if the end of the current line matches the start of the potential new line
                    if (cleaned_outline.back().get_end() == line.get_start()) {
                        // Replace the end of the previous line
                        cleaned_outline.back().set_end(line.get_end());
                    }
                } else {
                    // Add the line
                    cleaned_outline.push_back(line);
                }
            }

            return cleaned_outline;
        }

    public:

        // Constructors

        using LineType::LineType;

        // Accessors

        Point get_finish() const {
            return this->finish;
        }

        // Helpers

        void construct_lines() {

            // Collect all points that exist on the outline

            build_edge_points();

#ifdef DEBUG_OUTLINE
            std::cout << "  Found Outline Points : " << this->edge.size() << " points" << std::endl;
#endif

            // Find outline edges on cardinal directions

            const std::vector<std::vector<int>> cardinal_points = {
                { 0, -1 }, { 1, 0 }, { 0, 1 }, { -1, 0 }
            };

            this->cardinals = build_edge_part(cardinal_points);

            // Find outline edges on diagonal directions

            const std::vector<std::vector<int>> diagonal_points = {
                { 1, -1 }, { 1, 1 }, { -1, 1 }, { -1, -1 }
            };

            this->diagonals = build_edge_part(diagonal_points);

#ifdef DEBUG_OUTLINE
            std::cout << "  Generated Outline Parts : " << this->cardinals.size() << " + " << this->diagonals.size() << " lines" << std::endl;
#endif

            // Remove unecessary diagonal lines (favouring cardinal ones)

            std::vector<Line> pruned_diagonals = prune_diagonals();

#ifdef DEBUG_OUTLINE
            std::cout << "  Removed Unnecessary Lines : " << this->diagonals.size() - pruned_diagonals.size() << " lines removed" << std::endl;
#endif

            // Concatenate the lists

            this->lines.reserve(this->cardinals.size() + pruned_diagonals.size());
            this->lines.insert(this->lines.end(), this->cardinals.begin(), this->cardinals.end());
            this->lines.insert(this->lines.end(), pruned_diagonals.begin(), pruned_diagonals.end());

#ifdef DEBUG_OUTLINE
            std::cout << "  Found Outline : " << lines.size() << " lines" << std::endl;
#endif

            // Order lines

            this->lines = order_lines();

            // Set finishing point, so Infill can continue from there rather than (0, 0)

            this->finish = this->lines.back().get_end();

#ifdef DEBUG_OUTLINE
            std::cout << "  Ordered Outline" << std::endl;
#endif

            // Clean the outline

            this->lines = clean_outline();

#ifdef DEBUG_OUTLINE
            std::cout << "  Cleaned Outline" << std::endl;
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

            generate_stats();

#ifdef DEBUG_OUTLINE
            std::cout << "  Outline Stats Calculated" << std::endl;
#endif
        }

        // Overrides
};

#endif // outline_h
