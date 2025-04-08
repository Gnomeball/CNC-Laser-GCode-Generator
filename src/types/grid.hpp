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

        Grid(const int height, const int width)
        : height(height), width(width) {
            grid = std::vector<bool>(height * width);
            for (int y = 0; y < this->height; y++) {
                for (int x = 0; x < this->width; x++) {
                    this->grid.push_back(0);
                }
            }
        }

        // Accessors

        int get_height() const {
            return this->height;
        }

        int get_width() const {
            return this->width;
        }

        bool get_pixel(const int x, const int y) const {
            return this->grid.at(y * width + x);
        }

        void set_pixel(const int x, const int y, const int value) {
            this->grid.at(y * width + x) = value;
        }

        // Helpers

        // Overrides
};

#endif // grid_h
