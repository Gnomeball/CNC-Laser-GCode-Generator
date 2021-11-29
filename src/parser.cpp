// #include <Magick++.h>
#include <fstream>
#include <iostream>
#include <string>

#include "gcode.hpp"
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
    this->has_outline = false;
    this->has_infill = false;
    this->build_grids();
}

Parser::~Parser() {
    // delete[] this->master.get_grid();
    // delete[] this->edge.get_grid();
    // delete[] this->infill.get_grid();
}

int Parser::get_outline_speed() {
    return this->outline_speed;
}

int Parser::get_infill_speed() {
    return this->infill_speed;
}

int Parser::get_travel_speed() {
    return this->travel_speed;
}

int Parser::get_laser_power() {
    return this->laser_power;
}

void Parser::build_grids() {
    const Magick::Geometry &geometry = this->image.size();
    this->width = geometry.width();
    this->height = geometry.height();

    this->master = Grid(height, width);
    this->edge = Grid(height, width);
    this->infill = Grid(height, width);

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            Magick::Color pixel(this->image.pixelColor(x, y));
            int alpha = pixel.quantumAlpha();
            this->master.set_pixel(x, this->height - 1 - y, alpha);
        }
    }

    // todo: de-artefact the master grid

    build_edge_grid();
    build_infill_grid();
}

void Parser::build_edge_grid() {
    // * for every cell in master if it's a 1 but has a 0 next to it, it's an edge
    // for now we are ignoring the very edge of the image
    for (int y = 1; y < this->height - 1; y++) {
        for (int x = 1; x < this->width - 1; x++) {
            if (this->master.get_pixel(x, y) == 1) { // * this is a 1
                int neighbours[4][2]{{x, y - 1}, {x + 1, y}, {x, y + 1}, {x - 1, y}};
                for (auto n : neighbours) {
                    if (this->master.get_pixel(n[0], n[1]) == 0) {
                        this->edge.set_pixel(x, y, 1);
                        break;
                    }
                }
            }
        }
    }
}

void Parser::build_infill_grid() {
    // * for every cell, if master != edge it's infill
    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            int value = this->master.get_pixel(x, y) != this->edge.get_pixel(x, y);
            this->infill.set_pixel(x, y, value);
        }
    }
}

void Parser::build_gcode_outline() {
    this->gcode_outline = Gcode(this->edge);
    this->gcode_outline.build(OUTLINE, this->outline_speed, this->travel_speed);
    this->has_outline = true;
}

void Parser::build_gcode_infill() {
    this->gcode_infill = Gcode(this->infill);
    this->gcode_infill.build(INFILL, this->infill_speed, this->travel_speed);
    this->has_infill = true;
}

void Parser::write_gcode_to_file() {
    std::string output_file_name =
        this->file_name.substr(0, this->file_name.find(".")).append(".gcode");
    std::ofstream output("../gcode/" + output_file_name);
    output.flush();

    output << "; ##############################################################################\n";
    output << "; #                                                                            #\n";
    output << "; #                     Not So Terible GCode Generator Mk8                     #\n";
    output << "; #                       ~~~~  Now with more C++!  ~~~~                       #\n";
    output << "; #                                                                            #\n";
    output << "; ##############################################################################\n";
    output << ";\n";
    output << "; File : " << this->file_name << "\n";
    output << ";\n";
    output << "; Outline Speed : " << outline_speed << "\n";
    output << "; Infill Speed  : " << infill_speed << "\n";
    output << "; Travel Speed  : " << travel_speed << "\n";
    output << "; Laser Power   : " << laser_power << "\n";
    output << ";\n";

    if (this->has_outline) {
        output << "; Outline Stats\n";
        output << ";\n";
        output << "; Burn Distance   : " << gcode_outline.get_burn_distance() << "mm\n";
        output << "; Travel Distance : " << gcode_outline.get_travel_distance() << "mm\n";
        output << "; Efficiency      : " << gcode_outline.get_efficiency() << "%\n";
        output << "; Estimated Time  : " << gcode_outline.get_estimated_time() << "s\n";
        output << ";\n";
    }

    if (this->has_infill) {
        output << "; Infill Stats\n";
        output << ";\n";
        output << "; Burn Distance   : " << gcode_infill.get_burn_distance() << "mm\n";
        output << "; Travel Distance : " << gcode_infill.get_travel_distance() << "mm\n";
        output << "; Efficiency      : " << gcode_infill.get_efficiency() << "%\n";
        output << "; Estimated Time  : " << gcode_infill.get_estimated_time() << "s\n";
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

    if (this->has_outline) {
        output << ";\n";
        output << "; OUTLINE START\n";
        output << ";\n";
        this->gcode_outline.write_to_file(output, laser_power, outline_speed, travel_speed);
        output << ";\n";
        output << "; OUTLINE END\n";
        output << ";\n";
    }

    if (this->has_infill) {
        output << ";\n";
        output << "; INFILL START\n";
        output << ";\n";
        this->gcode_infill.write_to_file(output, laser_power, infill_speed, travel_speed);
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
}

void Parser::write_grid_to_file() {
    std::ofstream master_file("../grids/grid_master.txt");
    std::ofstream edge_file("../grids/grid_edge.txt");
    std::ofstream infill_file("../grids/grid_infill.txt");
    master_file.flush();
    edge_file.flush();
    infill_file.flush();

    for (int y = 0; y < this->height; y++) {
        for (int x = 0; x < this->width; x++) {
            master_file << this->master.get_pixel(x, y);
            edge_file << this->edge.get_pixel(x, y);
            infill_file << this->infill.get_pixel(x, y);
        }
        master_file << "\n";
        edge_file << "\n";
        infill_file << "\n";
    }

    master_file.close();
    edge_file.close();
    infill_file.close();
}
