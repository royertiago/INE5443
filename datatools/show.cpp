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
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line

#include <getopt.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cstdio>
#include <cstring>
#include "pr/data_set.h"
#include "util/cv.h"

std::string colors = "red,green,blue,yellow,magenta,cyan,black";
std::map< std::string, std::string > color_map;

namespace command_line {
    int width = 300, height = 300;
    double radius = 6;

    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"width", required_argument, 0, 'w'},
            {"height", required_argument, 0, 'e'},
            {"radius", required_argument, 0, 'r'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        while( (opt = getopt_long( argc, argv, "w:e:r:h",
                    options, &dummy_option_index
                )) != -1 ) {
            switch( opt ) {
                case 'w':
                    if( std::sscanf( optarg, "%u", &width ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( width <= 0 ) {
                        std::fprintf( stderr, "Width must be positive.\n" );
                        std::exit(1);
                    }
                    break;
                case 'e':
                    if( std::sscanf( optarg, "%u", &height ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( height <= 0 ) {
                        std::fprintf( stderr, "Height must be positive.\n" );
                        std::exit(1);
                    }
                    break;
                case 'r':
                    if( std::sscanf( optarg, "%lf", &radius ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( radius <= 0 ) {
                        std::fprintf( stderr, "Radius must be positive.\n" );
                        std::exit(1);
                    }
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
    DataSet dataset = DataSet::parse( stdin );

    if( dataset.attribute_count() != 2 ) {
        std::fprintf( stderr, "Only datasets with two attributes are accepted.\n" );
        std::exit(1);
    }
    if( dataset.category_count() != 1 ) {
        std::fprintf( stderr, "Only datasets with one category are accepted.\n" );
        std::exit(1);
    }

    cv::Mat img( command_line::height, command_line::width, CV_8UC3 );
    util::show_dataset( img, dataset, command_line::radius );

    cv::namedWindow( "Dataset", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Dataset", img );
    cv::waitKey(0);

    return 0;
}
