#ifndef stats_h
#define stats_h

#include <sstream>
#include <string>
#include <vector>

#include "../types/line.hpp"
#include "../types/point.hpp"

class Stats {

    private:

        double burn_distance;

        double travel_distance;

        double efficiency;

        double time;

        // Helpers

        void calculate(const std::vector<Line> lines, const Point start, const double burn_speed, const double travel_speed) {
            // Distances
            Point last = start;

            for (Line line : lines) {
                // travel
                Line travel(last, line.get_start());
                this->travel_distance += (travel.length() / 10);
                // burn
                this->burn_distance += (line.length() / 10);
                // next
                last = line.get_end();
            }

            // Efficiency
            this->efficiency = this->burn_distance / (this->burn_distance + this->travel_distance) * 100;

            // Time
            this->time = (this->burn_distance / burn_speed * 60) + (this->travel_distance / travel_speed * 60);
        }

        double rounded_efficiency() {
            return std::ceil(this->efficiency * 100.0) / 100.0;
        }

    public:

        // Constructors

        Stats() {} // default

        Stats(const std::vector<Line> lines, const double burn_speed, const double travel_speed)
        : burn_distance(0), travel_distance(0) {
            this->calculate(lines, Point(0, 0), burn_speed, travel_speed);
        }

        Stats(const std::vector<Line> lines, const Point start, const double burn_speed, const double travel_speed)
        : burn_distance(0), travel_distance(0) {
            this->calculate(lines, start, burn_speed, travel_speed);
        }

        // Accessors

        // Helpers

        std::string to_string() {
            std::stringstream output;

            output << ";\n";
            output << "; Burn Distance   : " << this->burn_distance / 10 << "cm\n";
            output << "; Travel Distance : " << this->travel_distance / 10 << "cm\n";
            output << "; Efficiency      : " << rounded_efficiency() << "%\n";
            output << "; Estimated Time  : " << this->time << "s\n";
            output << ";\n";

            return output.str();
        }

        // Overrides
};

#endif // stats_h
