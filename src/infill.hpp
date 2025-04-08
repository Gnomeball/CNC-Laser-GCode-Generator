#ifndef infill_h
#define infill_h

#include <iostream>
#include <vector>

#include "line_type.hpp"

#include "misc/debug.hpp"

#include "types/grid.hpp"
#include "types/line.hpp"
#include "types/point.hpp"

class Infill : public LineType {

    private:

    public:

        // Constructors

        using LineType::LineType;

        // Accessors

        // Helpers

        void construct_lines(const int density) {

            // Build lines

            Point p_start;
            Point p_end;

            int line_skip = 10 / density;

            for (int y = 1; y < this->grid.get_height() - 1; y += line_skip) {
                for (int x = 1; x < this->grid.get_width() - 1; x++) {
                    if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x - 1, y)) {
                        // this is the start of a line
                        p_start = Point(x, y);
                    }
                    if (this->grid.get_pixel(x, y) && !this->grid.get_pixel(x + 1, y)) {
                        // this is the end of a line
                        p_end = Point(x, y);
                        Line temp = Line(p_start, p_end);
                        if (temp.length() != 0) { // check for zero-length lines and discard them
                            this->lines.push_back(temp);
                        }
                    }
                }
            }

#ifdef DEBUG_INFILL
            std::cout << "  Found Infill : " << lines.size() << " lines" << std::endl;
            std::cout << "  Starting Infill at : " << this->start.to_string() << "\n";
#endif

            // Order lines

            this->lines = order_lines();

#ifdef DEBUG_INFILL
            std::cout << "  Ordered Infill" << std::endl;
#endif

            generate_stats();

#ifdef DEBUG_INFILL
            std::cout << "  Infill Stats Calculated" << std::endl;
#endif
        }

        // Overrides
};

#endif // infill_h
