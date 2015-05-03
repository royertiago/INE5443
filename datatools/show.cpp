namespace command_line {
    const char help_message[] =
"%s [options]\n"
"Visualize the bidimensional dataset read in stdin.\n"
"\n"
"Options:\n"
"\n"
"--width <N>\n"
"--height <N>\n"
"    Defines the width/height of the image.\n"
"    Default value: 300 (for both).\n"
"\n"
"--radius <F>\n"
"    Defines the radius of each circle, in pixels.\n" 
"    Default: 6.\n"
"\n"
"--noise <N>\n"
"    Enable generation of some noise in the dataset.\n"
"    This command generates N noise points.\n"
"\n"
"--noise-seed <N>\n"
"    Choose N as the seed for the noise generation algorithm.\n"
"    Default: Generate a seed and print to stdout.\n"
"\n"
"--noise-expand <F>\n"
"    Choose F as the expansion factor for the noise;\n"
"    that is, how much the noisy poins may diverge\n"
"    from the dataset's absolute borders.\n"
"    Default: 0.1\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>
#include <cstring>
#include "cmdline/args.hpp"
#include "pr/data_set.h"
#include "util/cv.h"

std::string colors = "red,green,blue,yellow,magenta,cyan,black";
std::map< std::string, std::string > color_map;

namespace command_line {
    int width = 300, height = 300;
    double radius = 6;

    int noise = 0;
    long long unsigned noise_seed;
    bool noise_seed_set = false;
    double noise_expand = 0.1;

    void parse( cmdline::args && args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--width" ) {
                args.range( 1 ) >> width;
                continue;
            }
            if( arg == "--height" ) {
                args.range( 1 ) >> height;
                continue;
            }
            if( arg == "--radius" ) {
                args.range( 1 ) >> radius;
                continue;
            }
            if( arg == "--noise" ) {
                args.range( 1 ) >> noise;
                continue;
            }
            if( arg == "--noise-seed" ) {
                args >> noise_seed;
                noise_seed_set = true;
                continue;
            }
            if( arg == "--noise-expand" ) {
                args >> noise_expand;
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
    DataSet dataset = DataSet::parse( stdin );

    if( dataset.attribute_count() != 2 ) {
        std::fprintf( stderr, "Only datasets with two attributes are accepted.\n" );
        std::exit(1);
    }
    if( dataset.category_count() != 1 ) {
        std::fprintf( stderr, "Only datasets with one category are accepted.\n" );
        std::exit(1);
    }

    if( command_line::noise != 0 ) {
        if( command_line::noise_seed_set ) {
            dataset.noise(
                command_line::noise,
                command_line::noise_expand,
                command_line::noise_seed
            );
        }
        else {
            long long unsigned seed = dataset.noise(
                command_line::noise,
                command_line::noise_expand
            );
            std::printf( "Noise seed: %llu\n", seed );
        }
    }

    cv::Mat img( command_line::height, command_line::width, CV_8UC3 );
    util::show_dataset( img, dataset, command_line::radius );

    cv::namedWindow( "Dataset", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Dataset", img );
    cv::waitKey(0);

    return 0;
}
