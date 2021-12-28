#include <cstdlib>
#include <string>
#include <iostream>

#include "parser.hpp"

int main(int argc, char *argv[]) {

    std::string file_name;

    if (argc == 1) {
        std::cout << "no file name given" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        file_name = argv[1];
        std::cout << "file : " << file_name << std::endl;
        std::ifstream infile(file_name);
        if (!infile.good()) {
            std::cout << "file cannot be read" << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    Parser p = Parser(file_name);

    p.write_grid_to_file();

    // p.build_gcode_outline();
    p.build_gcode_infill();

    p.write_gcode_to_file();

    return 0;
}
