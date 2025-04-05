#ifndef line_h
#define line_h

#include <cmath>
#include <sstream>
#include <string>

#include "point.hpp"

class Line {

    private:

        Point start;

        Point end;

    public:

        // Constructors

        Line() {} // default

        Line(Point start, Point end)
        : start(start), end(end) {}

        // Accessors

        Point get_start() {
            return this->start;
        }

        Point get_end() {
            return this->end;
        }

        // Helpers

        void swap_points() {
            Point temp = this->start;
            this->start = this->end;
            this->end = temp;
        }

        double length() {
            double width = std::abs(this->start.get_x() - this->end.get_x());
            double height = std::abs(this->start.get_y() - this->end.get_y());
            return std::sqrt(std::pow(width, 2) + std::pow(height, 2));
        }

        double angle() {
            double width = std::abs(this->start.get_x() - this->end.get_x());
            double height = std::abs(this->start.get_y() - this->end.get_y());
            return std::atan(height / width);
        }

        std::string to_string(const int burn_power, const int burn_speed, const int travel_speed) {
            std::stringstream output;

            // go to start
            output << "G90 G1 X" << (double)this->start.get_x() / 10 << " Y" << (double)this->start.get_y() / 10 << " F" << travel_speed << "\n";

            // on
            output << "M03 S" << burn_power << "\n";

            // go to end
            output << "G90 G1 X" << (double)this->end.get_x() / 10 << " Y" << (double)this->end.get_y() / 10 << " F" << burn_speed << "\n";

            // off
            output << "M05" << "\n";

            return output.str();
        }

        // Overrides
};

#endif // line_h
