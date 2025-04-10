#ifndef config_h
#define config_h

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

#include "../lib/toml.hpp"

class Config {

    private:

        int laser_power;
        int laser_power_normalised;
        const int laser_power_low = 50;
        const int laser_power_high = 100;

        int travel_speed;
        const int travel_speed_low = 400;
        const int travel_speed_high = 750;

        bool has_outline;
        int outline_speed;
        const int outline_speed_low = 40;
        const int outline_speed_high = 80;

        bool has_infill;
        int infill_speed;
        const int infill_speed_low = 40;
        const int infill_speed_high = 80;

        int density;
        const std::vector<int> accepted_density = { 10, 5, 2, 1 };

        bool error_found = false;

        // Helpers

        bool out_of_bounds(const int value, const int low, const int high) const {
            return value < low || value > high;
        }

        bool validate_density() {
            for (const int value : this->accepted_density) {
                if (value == this->density) {
                    return true;
                }
            }
            return false;
        }

        void error(const std::string message) {
            std::cout << "Config Error : " << message << std::endl
                      << "See Config File for more information" << std::endl;
            this->error_found = true;
        }

        void validate() {
            // Laser Power
            if (this->out_of_bounds(this->laser_power,
                this->laser_power_low, this->laser_power_high)) {
                error("Requested Laser Power is out of bounds");
            }

            // Travel Speed
            if (this->out_of_bounds(this->travel_speed,
                this->travel_speed_low, this->travel_speed_high)) {
                error("Requested Travel Speed is out of bounds");
            }

            // Outline Speed
            if (this->out_of_bounds(this->outline_speed,
                this->outline_speed_low, this->outline_speed_high)) {
                error("Requested Outline Speed is out of bounds");
            }

            // Infill_speed
            if (this->out_of_bounds(this->infill_speed,
                this->infill_speed_low, this->infill_speed_high)) {
                error("Requested Infill Speed is out of bounds");
            }

            // Density
            if (!this->validate_density()) {
                error("Requested Density is not an acceptable value");
            }

            // Exit if error
            if (this->error_found) {
                exit(EXIT_FAILURE);
            }
        }

    public:

        // Constructors

        Config()
        : laser_power(75), laser_power_normalised(192),
          travel_speed(500),
          has_outline(true), outline_speed(60),
          has_infill(true), infill_speed(60),
          density(5) {
            // Default config provided when no file is given
        }

        Config(const std::string config_file) {

            // User customised config

            auto data = toml::parse(config_file);

            this->laser_power = toml::find<int>(data, "laser_power");

            this->travel_speed = toml::find<int>(data, "travel_speed");

            this->has_outline = toml::find<bool>(data, "has_outline");
            this->outline_speed = toml::find<int>(data, "outline_speed");

            this->has_infill = toml::find<bool>(data, "has_infill");
            this->infill_speed = toml::find<int>(data, "infill_speed");

            this->density = toml::find<int>(data, "density");

            this->validate();

            // Normalise Laser Power on 0-255 scale
            this->laser_power_normalised = std::ceil(255 * (double)this->laser_power / 100);
        }

        // Accessors

        int get_laser_power() {
            return this->laser_power_normalised;
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

            output << "  Laser Power   = " << this->laser_power << "%\n";

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
