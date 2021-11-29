#ifndef grid_h
#define grid_h

class Grid {
    // Fields
    int height;
    int width;
    bool *grid;

public:
    // Constructor(s) / Destructor(s)
    Grid() = default;
    Grid(int height, int width);
    ~Grid() = default;

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
