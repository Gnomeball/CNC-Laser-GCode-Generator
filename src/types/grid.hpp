#ifndef grid_h
#define grid_h

#include <vector>

class Grid {

    private:

        int height;

        int width;

        std::vector<bool> grid;

        std::vector<std::vector<int>> artefacts;

        // Helpers

        int count_neighbours(int x, int y) {

            std::vector<bool> neighbours = {
                // Cardinal
                this->get_pixel(x - 1, y), this->get_pixel(x, y - 1),
                this->get_pixel(x + 1, y), this->get_pixel(x, y + 1),
                // Diagonal
                // this->get_pixel(x - 1, y - 1), this->get_pixel(x - 1, y + 1),
                // this->get_pixel(x + 1, y - 1), this->get_pixel(x + 1, y + 1)
            };

            int count = 0;

            for (bool b : neighbours) {
                if (b) {
                    count += 1;
                }
            }

            return count;
        }

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

        int count_artefacts() {
            this->artefacts = std::vector<std::vector<int>>();

            int artefacts_found = 0;

            for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
                    if (this->get_pixel(x, y) == true) {
                        if (this->count_neighbours(x, y) < 2) {
                            this->artefacts.push_back({ x, y });
                            artefacts_found++;
                        }
                    }
                }
            }

            return artefacts_found;
        }

        void de_artefact() {
            for (std::vector<int> artefact : this->artefacts) {
                this->set_pixel(artefact[0], artefact[1], 0);
            }
        }

        // Overrides
};

#endif // grid_h
