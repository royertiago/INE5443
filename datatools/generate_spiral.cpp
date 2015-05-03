namespace command_line {
    const char help_message[] =
"%s [options]\n"
"Generates a bidimensional spiral dataset.\n"
"The dataset is printed to stdout.\n"
"\n"
"Options:\n"
"\n"
"-c <N>, --spirals <N>\n"
"    Number of generated spirals.\n"
"    The start angle of every spiral will be uniformly distributed.\n"
"    Default value: 2.\n"
"\n"
"-e <N>, --entries <N>\n"
"    Number of entries per spiral.\n"
"    Default value: 200.\n"
"\n"
"--radial-start <F>\n"
"    Initial distance from the spiral's center.\n"
"    Default value: 1.0\n"
"\n"
"--angular-start <F>\n"
"    Angle of the spiral's first point, in radians.\n"
"    Default value: 0.0\n"
"\n"
"--radial-step <F>\n"
"    How much the angle will change in every step, in radians.\n"
"    Default value: 0.006.\n"
"\n"
"--angular-step <F>\n"
"    How much the radius will change in every step.\n"
"    Default value: 0.063.\n"
"\n"
"--radial-noise <F>\n"
"    The maximum amount of noise in the radius of the new point.\n"
"    Default value: 0.3\n"
"\n"
"--seed <N>\n"
"    Use N as the seed.\n"
"    If not set, gererate a seed according to the current time.\n"
"    The seed is always inserted in the dataset, as a commentary.\n"
"\n"
"--help\n"
"    Display this help and quit.\n"
;
} // namespace command_line


#include <chrono>
#include <cmath>
#include <cstdio>
#include <random>
#include "cmdline/args.hpp"
#include "pr/data_entry.h"
#include "pr/data_set.h"

namespace command_line {
    unsigned spirals = 2;
    unsigned entries = 200;
    double radial_start = 1.0;
    double radial_step = 0.006;
    double radial_noise = 0.1;
    double angular_start = 0.0;
    double angular_step = 0.063;
    const char * x_label = "X";
    const char * y_label = "Y";
    const char * c_label = "Category";

    std::uint_fast32_t seed =
        std::chrono::system_clock::now().time_since_epoch().count();

    void parse( cmdline::args && args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--spirals" ) {
                args.range( 1 ) >> spirals;
                continue;
            }
            if( arg == "--entries" ) {
                args.range( 1 ) >> entries;
                continue;
            }
            if( arg == "--radial-start" ) {
                args >> radial_start;
                continue;
            }
            if( arg == "--angular-start" ) {
                args >> angular_start;
                continue;
            }
            if( arg == "--radial-step" ) {
                args >> radial_step;
                continue;
            }
            if( arg == "--angular-step" ) {
                args >> angular_step;
                continue;
            }
            if( arg == "--radial-noise" ) {
                args.range(0) >> radial_noise;
                continue;
            }
            if( arg == "--seed" ) {
                args >> seed;
                continue;
            }
            if( arg == "--help" ) {
                std::printf( help_message, args.program_name().c_str() );
                std::exit(0);
            }
            std::fprintf( stderr, "Unknown parameter %s.\n", arg.c_str() );
            std::exit(1);
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    std::mt19937 rng(command_line::seed);
    std::vector< DataEntry > data;
    double shift = command_line::entries * command_line::radial_step +
        command_line::radial_start + command_line::radial_noise;

    auto radial_noise = [
        &rng,
        random = std::uniform_real_distribution<double>(
            -command_line::radial_noise,
            command_line::radial_noise
        )
    ]() mutable {
        return random(rng);
    };

    auto push = [&]( double angle, double radius, const char * name ) {
        double x = std::cos(angle) * radius;
        double y = std::sin(angle) * radius;
        x += shift;
        y += shift;
        data.push_back(DataEntry({x, y},{name}));
    };

    for( unsigned i = 0; i < command_line::entries; i++ )
    for( unsigned j = 0; j < command_line::spirals; j++ ) {
        char name[2] = {(char) ('A' + j)};
        double angle = j * (2 * 3.14159265358979)/command_line::spirals +
             + command_line::angular_start + i * command_line::angular_step;
        double radius = command_line::radial_start + radial_noise() +
            i * command_line::radial_step;
        push( angle, radius, name );
    }

    DataSet( std::vector<std::string>{ command_line::x_label, command_line::y_label },
             std::vector<std::string>{ std::string(command_line::c_label) },
             std::move(data)
           ).write(stdout);
}
