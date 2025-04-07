#ifndef stats_h
#define stats_h

#include <sstream>
#include <string>
#include <vector>

#include "../types/line.hpp"

class Stats {

    private:

        double burn_distance;

        double travel_distance;

        double efficiency;

        double time;

        void calculate(std::vector<Line> lines, double burn_speed, double travel_speed) {
            // Distances
            Point last = Point(0, 0);
            for (Line l : lines) {
                // travel
                Line travel(last, l.get_start());
                this->travel_distance += (travel.length() / 10);
                // burn
                this->burn_distance += (l.length() / 10);
                // next
                last = l.get_end();
            }

            // Efficiency
            this->efficiency = this->burn_distance / (this->burn_distance + this->travel_distance) * 100;

            // Time
            this->time = (this->burn_distance / burn_speed * 60) + (this->travel_distance / travel_speed * 60);
        }

        double get_burn_distance() {
            return this->burn_distance;
        }

        double get_travel_distance() {
            return this->travel_distance;
        }

        double rounded_efficiency() {
            return std::ceil(this->efficiency * 100.0) / 100.0;
        }

        double get_time() {
            return this->time;
        }

    public:

        // Constructors

        Stats() {} // default

        Stats(std::vector<Line> lines, double burn_speed, double travel_speed)
        : burn_distance(0), travel_distance(0) {
            calculate(lines, burn_speed, travel_speed);
        }

        // Accessors

        // Helpers

        std::string to_string(std::string label) {
            std::stringstream output;

            output << "; " << label << " Stats\n";
            output << ";\n";
            output << "; Burn Distance   : " << this->burn_distance / 10 << "cm\n";
            output << "; Travel Distance : " << this->travel_distance / 10 << "cm\n";
            output << "; Efficiency      : " << rounded_efficiency() << "%\n";
            output << "; Estimated Time  : " << this->time << "\n";
            output << ";\n";

            return output.str();
        }

        // Overrides
};

#endif // stats_h
