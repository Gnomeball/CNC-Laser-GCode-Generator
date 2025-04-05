#ifndef grid_h
#define grid_h

#include <vector>

class Grid {

    private:

        int height;

        int width;

        std::vector<bool> grid;

    public:

        // Constructors

        Grid() {} // default;

        Grid(int height, int width)
        : height(height), width(width) {
            grid = std::vector<bool>(height * width);
            // todo: must be a better way
            for (int y = 0; y < this->height; y++) {
                for (int x = 0; x < this->width; x++) {
                    this->grid.push_back(0);
                }
            }
        }

        // Accessors

        int get_height() {
            return this->height;
        }

        int get_width() {
            return this->width;
        }

        // bool *get_grid();

        bool get_pixel(int x, int y) {
            return this->grid.at(y * width + x);
        }

        void set_pixel(int x, int y, int value) {
            this->grid.at(y * width + x) = value;
        }

        // Helpers

        // Overrides
};

#endif // grid_h
