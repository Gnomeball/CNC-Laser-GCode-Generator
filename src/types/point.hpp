#ifndef point_h
#define point_h

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

        // Overrides

        bool operator==(Point other) const {
            return this->x == other.x && this->y == other.y;
        }

        bool operator!=(Point other) const {
            return this->x != other.x || this->y != other.y;
        }
};

#endif // point_h
