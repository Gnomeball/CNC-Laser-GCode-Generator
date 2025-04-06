#ifndef point_h
#define point_h

#include <sstream>
#include <string>

class Point {

    private:

        int x;

        int y;

    public:

        // Constructors

        Point(void) {} // default

        Point(int x, int y)
        : x(x), y(y) {}

        // Accessors

        int get_x() {
            return this->x;
        }

        int get_y() {
            return this->y;
        }

        // Helpers

        std::string to_string() {
            std::stringstream output;
            output << "(" << this->x << ", " << this->y << ")";
            return output.str();
        }

        // Overrides

        bool operator==(Point other) const {
            return this->x == other.x && this->y == other.y;
        }

        bool operator!=(Point other) const {
            return this->x != other.x || this->y != other.y;
        }
};

#endif // point_h
