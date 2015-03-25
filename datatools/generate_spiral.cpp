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
#include <getopt.h>
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

    /* We will use freopen to reset stdin and stdout
     * in the presence of --input or --output options. */
    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"spirals", required_argument, 0, 'c'},
            {"entries", required_argument, 0, 'e'},
            {"radial-start", required_argument, 0, 'd'},
            {"angular-start", required_argument, 0, 't'},
            {"radial-step", required_argument, 0, 'r'},
            {"angular-step", required_argument, 0, 'a'},
            {"radial-noise", required_argument, 0, 'n'},
            {"seed", required_argument, 0, 's'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        unsigned long long tmp;
        while( (opt = getopt_long( argc, argv, "c:e:d:t:r:a:n:s:h",
                    options, &dummy_option_index
                )) != -1 ) {
            switch( opt ) {
                case 'c':
                    if( std::sscanf(optarg, "%u", &spirals) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( spirals <= 0 ) {
                        std::fprintf( stderr, "Number of spirals must be poistive.\n" );
                        std::exit(1);
                    }
                    break;
                case 'e':
                    if( std::sscanf(optarg, "%u", &entries) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(0);
                    }
                    if( entries <= 0 ) {
                        std::fprintf( stderr, "Number of entries must be poistive.\n" );
                        std::exit(1);
                    }
                    break;
                case 'd':
                    if( std::sscanf(optarg, "%lf", &radial_start) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 't':
                    if( std::sscanf(optarg, "%lf", &angular_start) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'r':
                    if( std::sscanf(optarg, "%lf", &radial_step) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'a':
                    if( std::sscanf(optarg, "%lf", &angular_step) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'n':
                    if( std::sscanf(optarg, "%lf", &radial_noise) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( radial_noise < 0 ) {
                        std::fprintf( stderr, "Radial noise must not be negative.\n" );
                        std::exit(1);
                    }
                    break;
                case 's':
                    if( std::sscanf(optarg, "%llu", &tmp) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    seed = tmp;
                    break;
                case 'h':
                    std::printf( help_message, argv[0] );
                    std::exit(0);
                    break;
                default:
                    std::fprintf( stderr, "Unknown parameter %c\n", optopt );
                    std::exit(1);
            }
        }
    } // void parse(int, char**)
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( argc, argv );

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
        char name[2] = {(char) 'A' + j};
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
