#include <string>

#include "parser.hpp"

int main() {

    std::string string = "../images/Duck.png";
    Parser p = Parser(string);

    p.write_grid_to_file();

    // p.build_gcode_outline();
    p.build_gcode_infill();

    p.write_gcode_to_file();

    return 0;
}
