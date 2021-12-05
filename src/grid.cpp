#include <iostream>
#include <vector>

#include "grid.hpp"

Grid::Grid(int height, int width) : height(height), width(width) {
    grid = std::vector<bool>(height * width);
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            this->grid.push_back(0);
        }
    }
}

int Grid::get_height() {
    return this->height;
}

int Grid::get_width() {
    return this->width;
}

bool Grid::get_pixel(int x, int y) {
    return this->grid.at(y * width + x);
}

void Grid::set_pixel(int x, int y, int value) {
    this->grid.at(y * width + x) = value;
}
