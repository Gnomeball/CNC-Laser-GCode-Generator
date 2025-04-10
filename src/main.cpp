#include <cstdlib>
#include <iostream>
#include <string>

#include "misc/config.hpp"
#include "misc/debug.hpp"

#include "types/point.hpp"

#include "parser.hpp"

int main(int argc, char *argv[]) {

    std::string image_file;
    std::string config_file;

    Config c = Config();

    if (argc == 1) {
        std::cout << "No Image file given" << std::endl;
        exit(EXIT_FAILURE);
    } else if (argc >= 2) {
        // Image only given
        image_file = argv[1];
        std::cout << "Input Image File is : " << image_file << std::endl;

        std::ifstream image_file_in(image_file);
        if (!image_file_in.good()) {
            std::cout << "Image File Cannot be Read" << std::endl;
            exit(EXIT_FAILURE);
        }

        std::cout << "Using Default Config" << std::endl;
    }

    if (argc == 3) {
        // Image and Config given
        config_file = argv[2];
        std::cout << "User Specified Config File is : " << config_file << std::endl;

        std::ifstream config_file_in(config_file);
        if (!config_file_in.good()) {
            std::cout << "Config File Cannot be Read" << std::endl;
            exit(EXIT_FAILURE);
        }

        Config c = Config(config_file);
    }


#ifdef DEBUG_CONFIG_PRINT_OUT
    std::cout << c.to_string();
#endif

    Parser p = Parser(image_file, c);

#ifdef DEBUG_GRID_PRINT_OUT
    p.write_grid_to_file();
#endif

    if (c.get_has_outline()) {
        p.build_gcode_outline();
    }

    Point outline_finish = c.get_has_outline() ? p.get_outline_finish() : Point(0, 0);

    if (c.get_has_infill()) {
        p.build_gcode_infill(outline_finish);
    }

    p.write_gcode_to_file();

    return 0;
}
