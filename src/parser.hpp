#ifndef parser_h
#define parser_h

#include <fstream>
#include <iostream>
#include <string>

#include <Magick++.h>

#include "misc/config.hpp"

#include "types/grid.hpp"

#include "infill.hpp"

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

        // Outline outline
        bool has_outline;
        const int outline_speed;

        Infill infill;
        bool has_infill;
        const int infill_speed;

        const int density;

        void build_grids() {
            const Magick::Geometry &geometry = this->image.size();
            this->width = geometry.width() + 2; // * padding
            this->height = geometry.height() + 2;

            this->master_grid = Grid(height, width);
            this->outline_grid = Grid(height, width);
            this->infill_grid = Grid(height, width);

            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    Magick::Color pixel(image.pixelColor(x, y));
                    int alpha = pixel.quantumAlpha();
                    this->master_grid.set_pixel(x + 1, height - y, alpha);
                }
            }

#ifdef DEBUG_MASTER_GRID
            std::cout << "Master Grid Created" << std::endl;
#endif

            this->de_artefact();

            this->build_edge_grid();
            this->build_infill_grid();
        }

        void de_artefact() {

            int removed = 0;
            Grid temp(height, width);

            for (int y = 1; y < height - 1; y++) {
                for (int x = 1; x < width - 1; x++) {
                    if (master_grid.get_pixel(x, y)) { // * this is a 1
                        int neighbours[4][2]{ { x, y - 1 }, { x + 1, y }, { x, y + 1 }, { x - 1, y } };
                        int count = 0;
                        for (auto n : neighbours)
                            count += master_grid.get_pixel(n[0], n[1]);
                        temp.set_pixel(x, y, count > 1); // * surrounded by 0's
                        removed += count <= 1;
                    }
                }
            }

            this->master_grid = temp;

#ifdef DEBUG_MASTER_GRID
            std::cout << "Master Grid De-Artefacted : " << removed << " pixels removed" << std::endl;
#endif
        }

        void build_edge_grid() {
            // * for every cell in master if it's a 1 but has a 0 next to it, it's an edge
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
#ifdef DEBUG_OUTLINE_GRID
            std::cout << "Outline Grid Created" << std::endl;
#endif
        }

        void build_infill_grid() {
            // * for every cell, if master != edge it's infill
            for (int y = 0; y < height; y++) {
                for (int x = 0; x < width; x++) {
                    int value = master_grid.get_pixel(x, y) != outline_grid.get_pixel(x, y);
                    infill_grid.set_pixel(x, y, value);
                }
            }
#ifdef DEBUG_INFILL_GRID
            std::cout << "Infill Grid Created" << std::endl;
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

        // Helpers

        void build_gcode_outline() {
            // this->outline = Gcode(this->outline_grid);
            // this->outline.build(OUTLINE, this->outline_speed, this->travel_speed);
        }

        void build_gcode_infill() {
            this->infill = Infill(infill_grid, infill_speed, laser_power, travel_speed);
            this->infill.construct_lines();
            this->infill.calculate_stats();
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
            output << "; Outline Speed : " << outline_speed << "\n";
            output << "; Infill Speed  : " << infill_speed << "\n";
            output << "; Travel Speed  : " << travel_speed << "\n";
            output << "; Laser Power   : " << laser_power << "\n";
            output << ";\n";

            // if (has_outline) {
            //     output << "; Outline Stats\n";
            //     output << ";\n";
            //     output << "; Burn Distance   : " << gcode_outline.get_burn_distance() << "mm\n";
            //     output << "; Travel Distance : " << gcode_outline.get_travel_distance() << "mm\n";
            //     output << "; Efficiency      : " << gcode_outline.get_efficiency() << "%\n";
            //     output << "; Estimated Time  : " << gcode_outline.get_time() << "s\n";
            //     output << ";\n";
            // }

            if (has_infill) {
                output << "; Infill Stats\n";
                output << ";\n";
                output << "; Burn Distance   : " << infill.get_stats().get_burn_distance() << "mm\n";
                output << "; Travel Distance : " << infill.get_stats().get_travel_distance() << "mm\n";
                output << "; Efficiency      : " << infill.get_stats().get_efficiency() << "%\n";
                output << "; Estimated Time  : " << infill.get_stats().get_time() << "s\n";
                output << ";\n";
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

            // if (has_outline) {
            //     output << ";\n";
            //     output << "; OUTLINE START\n";
            //     output << ";\n";
            //     gcode_outline.write_to_file(output, laser_power, outline_speed, travel_speed);
            //     output << ";\n";
            //     output << "; OUTLINE END\n";
            //     output << ";\n";
            // }

            if (has_infill) {
                output << ";\n";
                output << "; INFILL START\n";
                output << ";\n";
                infill.write_to_file(output);
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
#ifdef DEBUG_GCODE_FILE
            std::cout << "GCode File Written at : gcode/" << output_file_name << std::endl;
#endif
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
