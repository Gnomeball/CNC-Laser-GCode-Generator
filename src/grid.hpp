#ifndef grid_h
#define grid_h

#include <vector>

class Grid {
    // Fields
    int height;
    int width;
    std::vector<bool> grid;

public:
    // Constructor(s) / Destructor(s)
    Grid() = default;
    Grid(int height, int width);

    // Member Functions
    int get_height();
    int get_width();
    // bool *get_grid();

    // Utility Functions
    bool get_pixel(int x, int y);
    void set_pixel(int x, int y, int value);

    // Overrides
};

#endif // grid_h
