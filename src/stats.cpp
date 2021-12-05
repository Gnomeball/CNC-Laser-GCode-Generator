#include <vector>

#include "line.hpp"
#include "point.hpp"
#include "stats.hpp"

Stats::Stats(std::vector<Line> lines, double speed, double travel)
    : burn_distance(0), travel_distance(0) {
    calculate(lines, speed, travel);
}

void Stats::calculate(std::vector<Line> lines, double speed, double travel) {
    // * Distances
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

    // * Efficiency
    this->efficiency = this->burn_distance / (this->burn_distance + this->travel_distance) * 100;

    // * Time
    this->time = (this->burn_distance / speed * 10) + (this->travel_distance / travel * 10);
}

double Stats::get_burn_distance() {
    return this->burn_distance;
}

double Stats::get_travel_distance() {
    return this->travel_distance;
}

double Stats::get_efficiency() {
    return this->efficiency;
}

int Stats::get_time() {
    return this->time;
}
