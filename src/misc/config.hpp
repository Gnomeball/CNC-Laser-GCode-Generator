#ifndef config_h
#define config_h

#include <sstream>
#include <string>

#include "../lib/toml.hpp"

class Config {

    private:

        int laser_power;

        int travel_speed;

        bool has_outline;
        int outline_speed;

        bool has_infill;
        int infill_speed;

        int density;

    public:

        // Constructors

        Config(std::string config_file) {

            auto data = toml::parse(config_file);

            this->laser_power = toml::find<int>(data, "laser_power");

            this->travel_speed = toml::find<int>(data, "travel_speed");

            this->has_outline = toml::find<bool>(data, "has_outline");
            this->outline_speed = toml::find<int>(data, "outline_speed");

            this->has_infill = toml::find<bool>(data, "has_infill");
            this->infill_speed = toml::find<int>(data, "infill_speed");

            this->density = toml::find<int>(data, "density");
        }

        // Accessors

        int get_laser_power() {
            return this->laser_power;
        }

        int get_travel_speed() {
            return this->travel_speed;
        }

        bool get_has_outline() {
            return this->has_outline;
        }

        int get_outline_speed() {
            return this->outline_speed;
        }

        bool get_has_infill() {
            return this->has_infill;
        }

        int get_infill_speed() {
            return this->infill_speed;
        }

        int get_density() {
            return this->density;
        }

        // Helpers

        std::string to_string() {
            std::stringstream output;

            output << "Current Config : " << "\n";

            output << "  Laser Power   = " << this->laser_power << "\n";

            output << "  Travel Speed  = " << this->travel_speed << "\n";

            output << "  Has Outline   = " << (this->has_outline ? "true" : "false") << "\n";
            output << "  Outline Speed = " << this->outline_speed << "\n";

            output << "  Has Infill    = " << (this->has_infill ? "true" : "false") << "\n";
            output << "  Infill Speed  = " << this->infill_speed << "\n";

            output << "  Density       = " << this->density << "\n";

            return output.str();
        }
};

#endif // config_h
