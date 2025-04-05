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
            int width = std::abs(this->start.get_x() - this->end.get_x());
            int height = std::abs(this->start.get_y() - this->end.get_y());
            return std::sqrt(std::pow(width, 2) + std::pow(height, 2));
        }

        double angle() {
            double width = std::abs(this->start.get_x() - this->end.get_x());
            double height = std::abs(this->start.get_y() - this->end.get_y());
            return std::atan(height / width);
        }

        std::string to_string(const int burn, const int speed, const int travel) {
            std::stringstream output;
            output << "G90 G1 X" << (double)this->start.get_x() / 10 << " Y" << (double)this->start.get_y() / 10 << " F" << travel << "\n";
            output << "M03 S" << burn << "\n";
            output << "G90 G1 X" << (double)this->end.get_x() / 10 << " Y" << (double)this->end.get_y() / 10 << " F" << speed << "\n";
            output << "M05" << "\n";
            return output.str();
        }

        // Overrides
};

#endif // line_h
