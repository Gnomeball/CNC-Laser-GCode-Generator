#ifndef parser_h
#define parser_h

#include <fstream>
#include <iostream>
#include <string>

#include <Magick++.h>
#include <vector>

#include "misc/config.hpp"
#include "misc/debug.hpp"

#include "types/grid.hpp"
#include "types/point.hpp"

#include "infill.hpp"
#include "outline.hpp"

class Parser {
        std::string file_name;

        Magick::Image image;

        int height;
        int width;

        Grid master_grid;
        Grid outline_grid;
        Grid infill_grid;

        const int laser_power;
        const int travel_speed;

        Outline outline;
        bool has_outline;
        const int outline_speed;

        Infill infill;
        bool has_infill;
        const int infill_speed;

        const int density;

        void build_grids() {

#ifdef DEBUG_GRIDS
            std::cout << "Creating Grids" << std::endl;
#endif

            // Setup

            // todo : canvas padding to centralise the image

            const Magick::Geometry &geometry = this->image.size();
            this->width = geometry.width() + 2; // padding
            this->height = geometry.height() + 2;

            this->master_grid = Grid(height, width);
            this->outline_grid = Grid(height, width);
            this->infill_grid = Grid(height, width);

            // Master Grid

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Magick::Color pixel(image.pixelColor(x, y));
                    int alpha = pixel.quantumAlpha();
                    this->master_grid.set_pixel(x + 1, height - y, alpha);
                }
            }

#ifdef DEBUG_GRIDS
            std::cout << "  Master Grid Created" << std::endl;
            int removed = 0;
#endif

            // We define an artefact as a pixel, with only one other pixel
            // neighbouring it in any of the four cardinal directions

            Grid temp(height, width);

            for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
                    if (master_grid.get_pixel(x, y)) { // this is a 1
                        std::vector<std::vector<int>> neighbours = {
                            { x, y - 1 },
                            { x + 1, y },
                            { x, y + 1 },
                            { x - 1, y }
                        };
                        int count = 0;
                        for (std::vector<int> n : neighbours) {
                            count += master_grid.get_pixel(n[0], n[1]);
                        }
                        temp.set_pixel(x, y, count > 1); // true if it has more than one neighbour
#ifdef DEBUG_GRIDS
                        removed += count <= 1; // impilicit false to the above
#endif
                    }
                }
            }

            this->master_grid = temp;

#ifdef DEBUG_GRIDS
            std::cout << "  Master Grid De-Artefacted : " << removed << " pixels removed" << std::endl;
#endif

            // Outline Grid

            // for every cell in master if it's a 1 but has a 0 next to it, it's an edge
            for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
                    if (master_grid.get_pixel(x, y)) { // * this is a 1
                        int neighbours[4][2]{ { x, y - 1 }, { x + 1, y }, { x, y + 1 }, { x - 1, y } };
                        for (auto n : neighbours) {
                            if (master_grid.get_pixel(n[0], n[1]) == 0) {
                                outline_grid.set_pixel(x, y, 1);
                                break;
                            }
                        }
                    }
                }
            }

#ifdef DEBUG_GRIDS
            std::cout << "  Outline Grid Created" << std::endl;
#endif

            // Infill Grid

            if (!this->has_outline) {
                // Output is only the infill, so we don't cut off the outline
                infill_grid = master_grid;
            } else {
                // for every cell, if master != edge, it's infill
                for (int y = 0; y < height; y++) {
                    for (int x = 0; x < width; x++) {
                        int value = master_grid.get_pixel(x, y) != outline_grid.get_pixel(x, y);
                        infill_grid.set_pixel(x, y, value);
                    }
                }
            }

#ifdef DEBUG_GRIDS
            std::cout << "  Infill Grid Created" << std::endl;
#endif
        }

    public:

        // Constructors

        Parser(std::string file_name, Config config)
        : laser_power(config.get_laser_power()),
          travel_speed(config.get_travel_speed()),
          has_outline(config.get_has_outline()),
          outline_speed(config.get_outline_speed()),
          has_infill(config.get_has_infill()),
          infill_speed(config.get_infill_speed()),
          density(config.get_density()) {
            try {
                // todo: add file extension check for .png
                Magick::Image img(file_name);
                this->image = img;
            } catch (Magick::Exception &error) {
                std::cout << error.what() << std::endl;
                exit(EXIT_FAILURE);
            }
            this->file_name = file_name.substr(file_name.rfind("/") + 1, file_name.rfind("."));
            this->build_grids();
        }

        // Accessors

        Point get_finish() {
            return this->outline.get_finish();
        }

        // Helpers

        void build_gcode_outline() {
            this->outline = Outline(outline_grid, laser_power, outline_speed, travel_speed);
            this->outline.construct_lines();
#ifdef DEBUG_INFILL
            std::cout << "  Outline Construction Complete" << std::endl;
#endif
        }

        void build_gcode_infill(Point start) {
            this->infill = Infill(infill_grid, start, laser_power, infill_speed, travel_speed);
            this->infill.construct_lines(this->density);
#ifdef DEBUG_OUTLINE
            std::cout << "  Infill Construction Complete" << std::endl;
#endif
        }

        void write_gcode_to_file() {
            std::string output_file_name = file_name.substr(0, file_name.find(".")).append(".gcode");
            std::ofstream output("gcode/" + output_file_name);
            output.flush();

            output << "; ##############################################################################\n";
            output << "; #                                                                            #\n";
            output << "; #                     Not So Terible GCode Generator Mk8                     #\n";
            output << "; #                       ~~~~  Now with more C++!  ~~~~                       #\n";
            output << "; #                                                                            #\n";
            output << "; ##############################################################################\n";

            output << ";\n";
            output << "; File : " << file_name << "\n";
            output << ";\n";
            output << "; Laser Power   : " << laser_power << "\n";
            output << "; Travel Speed  : " << travel_speed << "\n";
            if (has_outline)
                output << "; Outline Speed : " << outline_speed << "\n";
            if (has_infill)
                output << "; Infill Speed  : " << infill_speed << "\n";
            output << "; Line Density  : " << density << "\n";
            output << ";\n";

            if (has_outline) {
                outline.print_stats(output);
            }

            if (has_infill) {
                infill.print_stats(output);
            }

            output << "; ##############################################################################\n";
            output << "; #                                                                            #\n";
            output << "; #                             GCode Starts Here!                             #\n";
            output << "; #                                                                            #\n";
            output << "; ##############################################################################\n";

            output << ";\n";
            output << "; ABSOLUTE POSITIONING\n";
            output << ";\n";
            output << "G90\n";
            output << ";\n";
            output << "; IMAGE START\n";
            output << ";\n";

            // Image

            if (has_outline) {
                output << ";\n";
                output << "; OUTLINE START\n";
                output << ";\n";

                outline.write_to_file(output);

                output << ";\n";
                output << "M05 ; LASER OFF\n";

                output << ";\n";
                output << "; OUTLINE END\n";
                output << ";\n";
            }

            if (has_infill) {
                output << ";\n";
                output << "; INFILL START\n";
                output << ";\n";

                infill.write_to_file(output);

                output << ";\n";
                output << "M05 ; LASER OFF\n";

                output << ";\n";
                output << "; INFILL END\n";
                output << ";\n";
            }

            output << ";\n";
            output << "; IMAGE END\n";
            output << ";\n";
            output << "G28\n";
            output << ";\n";

            output << "; ##############################################################################\n";
            output << "; #                                                                            #\n";
            output << "; #                            GCode Finishes Here!                            #\n";
            output << "; #                                                                            #\n";
            output << "; ##############################################################################\n";

            output.close();

            std::cout << "GCode File Written at : gcode/" << output_file_name << std::endl;
        }

        void write_grid_to_file() {
            std::cout << "Writing Grids to File" << std::endl;

            std::ofstream master_file("grids/master_grid.txt");
            std::ofstream outline_file("grids/outline_grid.txt");
            std::ofstream infill_file("grids/infill_grid.txt");

            master_file.flush();
            outline_file.flush();
            infill_file.flush();

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    master_file << master_grid.get_pixel(x, y);
                    outline_file << outline_grid.get_pixel(x, y);
                    infill_file << infill_grid.get_pixel(x, y);
                }
                master_file << "\n";
                outline_file << "\n";
                infill_file << "\n";
            }

            master_file.close();
            outline_file.close();
            infill_file.close();
        }

        // Overrides
};

#endif // parser_h
