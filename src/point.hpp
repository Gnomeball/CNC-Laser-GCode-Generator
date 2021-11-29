#ifndef point_h
#define point_h

class Point {
    // Fields
    int x;
    int y;

public:
    // Constructor(s) / Destructor(s)
    Point() = default;
    Point(int x, int y);
    ~Point() = default;

    // Member Functions
    int get_x();
    int get_y();

    // Utility Functions

    // Overrides
};

#endif // point_h
