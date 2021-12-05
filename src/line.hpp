#ifndef line_h
#define line_h

#include <string>

#include "point.hpp"

class Line {
    // Fields
    Point start;
    Point end;

public:
    // Constructor(s) / Destructor(s)
    Line() = default;
    Line(Point start, Point end);
    ~Line() = default;

    // Member Functions
    Point get_start();
    Point get_end();

    // Utility Functions
    void swap_points();
    double length();
    double angle();
    std::string to_string(const int burn, const int speed, const int travel);

    // Overrides
};

#endif // line_h
