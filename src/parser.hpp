#ifndef parser_h
#define parser_h

#include <string>

#include <Magick++.h>

#include "types/grid.hpp"

#include "infill.hpp"

class Parser {
    std::string file_name;

    Magick::Image image;
    int height;
    int width;

    Grid grid_master;
    Grid grid_edge;
    Grid grid_infill;

    // Outline outline
    // bool has_outline;
    Infill infill;
    bool has_infill;

    const int outline_speed;
    const int infill_speed;
    const int travel_speed;
    const int laser_power;

    void build_grids();
    void de_artefact();
    void build_edge_grid();
    void build_infill_grid();

public:
    // Constructor(s) / Destructor(s)
    Parser(std::string file_name);
    ~Parser() = default;

    // Member Functions

    // Utility Functions
    // void build_gcode_outline();
    void build_gcode_infill();

    void write_gcode_to_file();
    void write_grid_to_file();

    // Overrides
};

#endif // parser_h
