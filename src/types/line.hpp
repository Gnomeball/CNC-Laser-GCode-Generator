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

        // used for outline
        bool is_continuation = false;

    public:

        // Constructors

        Line() {} // default

        Line(const Point start, const Point end)
        : start(start), end(end) {}

        // Accessors

        const Point get_start() const {
            return this->start;
        }

        const Point get_end() const {
            return this->end;
        }

        void set_continuation(const bool value) {
            this->is_continuation = value;
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
            double width = this->start.get_x() - this->end.get_x();
            double height = this->start.get_y() - this->end.get_y();
            return std::atan(height / width) * 180 / 3.14159;
        }

        std::string to_string(const int laser_power, const int burn_speed, const int travel_speed) {
            std::stringstream output;

            if (!this->is_continuation) {
                // off
                output << "M05" << "\n";

                // go to start
                output << "G90 G1"
                       << " X" << (double)this->start.get_x() / 10
                       << " Y" << (double)this->start.get_y() / 10
                       << " F" << travel_speed << "\n";

                // on
                output << "M03 S" << laser_power << "\n";
            }

            // go to end
            output << "G90 G1"
                   << " X" << (double)this->end.get_x() / 10
                   << " Y" << (double)this->end.get_y() / 10
                   << " F" << burn_speed << "\n";

            return output.str();
        }

        // Overrides

        bool operator==(const Line other) const {
            return (this->start == other.start && this->end == other.end) || (this->start == other.end && this->end == other.start);
        }
};

#endif // line_h
