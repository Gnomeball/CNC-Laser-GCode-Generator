// #include <Magick++.h>
#include <fstream>
#include <iostream>
#include <string>

#include "debug.hpp"
#include "grid.hpp"
#include "parser.hpp"

Parser::Parser(std::string file_name)
    : outline_speed(50), infill_speed(60), travel_speed(500), laser_power(192) {
    try {
        Magick::Image img(file_name);
        this->image = img;
    } catch (Magick::Exception &error) {
        std::cout << error.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    this->file_name = file_name.substr(file_name.rfind("/") + 1, file_name.rfind("."));
    // this->has_outline = false;
    this->has_infill = false;
    this->build_grids();
}

void Parser::build_grids() {
    const Magick::Geometry &geometry = this->image.size();
    this->width = geometry.width() + 2; // * padding
    this->height = geometry.height() + 2;

    this->grid_master = Grid(height, width);
    this->grid_edge = Grid(height, width);
    this->grid_infill = Grid(height, width);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            Magick::Color pixel(image.pixelColor(x, y));
            int alpha = pixel.quantumAlpha();
            this->grid_master.set_pixel(x + 1, height - y, alpha);
        }
    }

#ifdef DEBUG_PRINT_OUT
    std::cout << "Master Grid Created" << std::endl;
#endif

    de_artefact();

    build_edge_grid();
    build_infill_grid();
}

void Parser::de_artefact() {

    int removed = 0;
    Grid temp(height, width);

    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (grid_master.get_pixel(x, y)) { // * this is a 1
                int neighbours[4][2]{{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};
                int count = 0;
                for (auto n : neighbours)
                    count += grid_master.get_pixel(n[0], n[1]);
                temp.set_pixel(x, y, count > 1); // * surrounded by 0's
                removed += count <= 1;
            }
        }
    }

    this->grid_master = temp;

#ifdef DEBUG_PRINT_OUT
    std::cout << "Master Grid De-Artefacted : " << removed << " pixels removed" << std::endl;
#endif
}

void Parser::build_edge_grid() {
    // * for every cell in master if it's a 1 but has a 0 next to it, it's an edge
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            if (grid_master.get_pixel(x, y)) { // * this is a 1
                int neighbours[4][2]{{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};
                for (auto n : neighbours) {
                    if (grid_master.get_pixel(n[0], n[1]) == 0) {
                        grid_edge.set_pixel(x, y, 1);
                        break;
                    }
                }
            }
        }
    }
#ifdef DEBUG_PRINT_OUT
    std::cout << "Outline Grid Created" << std::endl;
#endif
}

void Parser::build_infill_grid() {
    // * for every cell, if master != edge it's infill
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int value = grid_master.get_pixel(x, y) != grid_edge.get_pixel(x, y);
            grid_infill.set_pixel(x, y, value);
        }
    }
#ifdef DEBUG_PRINT_OUT
    std::cout << "Infill Grid Created" << std::endl;
#endif
}

// void Parser::build_gcode_outline() {
//     this->gcode_outline = Gcode(this->grid_edge);
//     this->gcode_outline.build(OUTLINE, this->outline_speed, this->travel_speed);
//     this->has_outline = true;
// }

void Parser::build_gcode_infill() {
    this->infill = Infill(grid_infill, infill_speed, laser_power, travel_speed);
    this->infill.construct_lines();
    this->infill.calculate_stats();
    this->has_infill = true;
}

void Parser::write_gcode_to_file() {
    std::string output_file_name = file_name.substr(0, file_name.find(".")).append(".gcode");
    std::ofstream output("../gcode/" + output_file_name);
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
#ifdef DEBUG_PRINT_OUT
    std::cout << "GCode File Written" << std::endl;
#endif
}

void Parser::write_grid_to_file() {
    std::ofstream master_file("../grids/grid_master.txt");
    std::ofstream edge_file("../grids/grid_edge.txt");
    std::ofstream infill_file("../grids/grid_infill.txt");
    master_file.flush();
    edge_file.flush();
    infill_file.flush();

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            master_file << grid_master.get_pixel(x, y);
            edge_file << grid_edge.get_pixel(x, y);
            infill_file << grid_infill.get_pixel(x, y);
        }
        master_file << "\n";
        edge_file << "\n";
        infill_file << "\n";
    }

    master_file.close();
    edge_file.close();
    infill_file.close();
}
