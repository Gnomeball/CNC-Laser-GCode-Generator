#include <cstdlib>
#include <iostream>
#include <string>

#include "parser.hpp"

#include "misc/config.hpp"
#include "misc/debug.hpp"

#include "types/point.hpp"

std::string collect_image_path(char *path) {
    std::string image_file = path;
    std::cout << "Input Image File is : " << image_file << std::endl;

    std::ifstream image_file_in(image_file);
    if (!image_file_in.good()) {
        std::cout << "Image File Cannot be Read" << std::endl;
        exit(EXIT_FAILURE);
    }

    return image_file;
}

std::string collect_config_path(char *path) {
    std::string config_file = path;
    std::cout << "User Specified Config File is : " << config_file << std::endl;

    std::ifstream config_file_in(config_file);
    if (!config_file_in.good()) {
        std::cout << "Config File Cannot be Read" << std::endl;
        exit(EXIT_FAILURE);
    }

    return config_file;
}

int main(int argc, char *argv[]) {

    std::string image_file;
    std::string config_file = "none set (yet)";

    bool config_set = false;

    if (argc == 1) {
        std::cout << "No Image file given" << std::endl;
        exit(EXIT_FAILURE);
    } else if (argc >= 2) {
        // Image only given
        image_file = collect_image_path(argv[1]);
        if (argc >= 3) {
            // Image and Config given
            config_file = collect_config_path(argv[2]);
            config_set = true;
        } else {
            // No Config given
            std::cout << "Using Default Config" << std::endl;
        }
    }

    Config c = config_set ? Config(config_file) : Config();

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
