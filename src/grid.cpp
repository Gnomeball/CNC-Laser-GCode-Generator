#include "grid.hpp"

Grid::Grid(int height, int width) : height(height), width(width) {
    this->grid = new bool[height * width];

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            this->grid[y * this->width + x] = 0;
        }
    }

}

int Grid::get_height() {
    return this->height;
}

int Grid::get_width() {
    return this->width;
}

// bool *Grid::get_grid() {
//     return this->grid;
// }

bool Grid::get_pixel(int x, int y) {
    return grid[y * width + x];
}

void Grid::set_pixel(int x, int y, int value) {
    grid[y * width + x] = value;
}
