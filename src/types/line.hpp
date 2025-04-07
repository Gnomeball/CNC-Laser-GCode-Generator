#ifndef line_h
#define line_h

#include <cmath>
#include <sstream>
#include <string>
#include <vector>

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

        Line(Point start, Point end)
        : start(start), end(end) {}

        // Accessors

        Point get_start() {
            return this->start;
        }

        Point get_end() {
            return this->end;
        }

        void set_continuation(bool value) {
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

            // output << this->is_continuation << "\n";

            if (this->is_continuation) {
                // go to end
                output << "G90 G1 X" << (double)this->end.get_x() / 10 << " Y" << (double)this->end.get_y() / 10 << " F" << burn_speed << "\n";
            } else {
                // off
                output << "M05" << "\n";

                // go to start
                output << "G90 G1 X" << (double)this->start.get_x() / 10 << " Y" << (double)this->start.get_y() / 10 << " F" << travel_speed << "\n";

                // on
                output << "M03 S" << laser_power << "\n";

                // go to end
                output << "G90 G1 X" << (double)this->end.get_x() / 10 << " Y" << (double)this->end.get_y() / 10 << " F" << burn_speed << "\n";
            }

            return output.str();
        }

        // Overrides

        bool operator==(Line other) const {
            return (this->start == other.start && this->end == other.end) || (this->start == other.end && this->end == other.start);
        }
};

inline std::vector<Line> order_lines(std::vector<Line> lines, Point starting_point) {

    std::vector<Line> temp = std::vector<Line>();

    double start;
    double end;
    double distance_from_last;
    double minimum_travel = INFINITY;

    Line closest;
    int closest_index;

    temp.push_back(Line(Point(0, 0), starting_point));

    while (lines.size() > 0) {

        int index = 0;
        for (Line l : lines) {

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
        lines.erase(lines.begin() + closest_index);

        minimum_travel = INFINITY;
    }

    temp.erase(temp.begin());

    return temp;

}

#endif // line_h
