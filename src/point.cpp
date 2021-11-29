#include "point.hpp"

Point::Point(int x, int y) : x(x), y(y) {
    // Empty
}

int Point::get_x() {
    return this->x;
}

int Point::get_y() {
    return this->y;
}
