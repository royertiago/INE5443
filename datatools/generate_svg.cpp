namespace command_line {
    const char help_message[] =
"$0 [options]\n"
"Generate a SVG from the dataset read in stdin.\n"
"Output is printed on stdout.\n"
"The entry format is assumed to be <number>,<number>,<category>;\n"
"that is, this program only support 2D category visualization.\n"
"\n"
"Options:\n"
"\n"
"--width <N>\n"
"--height <N>\n"
"    Defines the width/height of the image.\n"
"    Default value: 300 (for both).\n"
"\n"
"--radius <F>\n"
"    Defines the radius of each circle plotted to the .svg.\n"
"    Default: 2.\n"
"\n"
"--color-list <comma-separated color list>\n"
"    Use the given colors for the categories for which no color was assigned\n"
"    using the option '--colors'.\n"
"    Default value: \"red,green,blue,yellow,magenta,cyan,black\"\n"
"\n"
"--more-colors <comma-separated color list>\n"
"    Appends the current color list with the given colors.\n"
"    The format is the same as with the option \"--color-list\".\n"
"    No checking is done to test wether there are repeated colors.\n"
"\n"
"--reserve-colors <comma-separated category list>\n"
"    Assign each of the categories in the given list\n"
"    to one of the colors in the color list, in the order they appear,\n"
"    before any other processing.\n"
"    This has the effect of reserving the first color\n"
"    to the first category of this list,\n"
"    the second color to the second category of this list, and so on.\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line

#include <getopt.h>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cfloat>
#include <map>
#include <string>
#include "pr/data_entry.h"
#include "pr/data_set.h"

std::string colors = "red,green,blue,yellow,magenta,cyan,black";
std::map< std::string, std::string > color_map;

std::string next_color() {
    if( colors == "" ) {
        std::fprintf( stderr, "Color list exausted.\n"
            "Please, provide more colors with \'--more-colors\' option.\n"
        );
        std::exit( 2 );
    }

    std::string::size_type pos = colors.find(',');
    std::string ret_val = colors.substr(0, pos);

    if( pos == std::string::npos )
        colors = "";
    else
        colors = colors.substr(pos + 1, std::string::npos);

    return ret_val;
}

std::string color_of( std::string category ) {
    std::string & ref = color_map[category];
    if( ref == "" )
        ref = next_color();

    return ref;
}

namespace command_line {
    unsigned width = 300, height = 300;
    double radius = 2;

    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"width", required_argument, 0, 'w'},
            {"height", required_argument, 0, 'e'},
            {"radius", required_argument, 0, 'r'},
            {"color-list", required_argument, 0, 'c'},
            {"more-colors", required_argument, 0, 'm'},
            {"reserve-colors", required_argument, 0, 'a'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        char * strtok_ret;
        while( (opt = getopt_long( argc, argv, "w:e:r:c:m:a:h",
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
                case 'c':
                    colors = optarg;
                    break;
                case 'm':
                    colors += ",";
                    colors += optarg;
                    break;
                case 'a':
                    strtok_ret = std::strtok(optarg, ",");
                    while( strtok_ret != nullptr ) {
                        (void) color_of( std::string(strtok_ret) );
                        strtok_ret = std::strtok(nullptr, ",");
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

    double min_x = DBL_MAX, min_y = DBL_MAX;
    double max_x = -DBL_MAX, max_y = -DBL_MAX;
    for( const DataEntry & entry : dataset ) {
        min_x = std::min(min_x, entry.attribute(0));
        min_y = std::min(min_y, entry.attribute(1));
        max_x = std::max(max_x, entry.attribute(0));
        max_y = std::max(max_y, entry.attribute(1));
    }

    double scale_x = command_line::width / (max_x - min_x);
    double scale_y = command_line::height / (max_y - min_y);

    std::printf("<svg xmlns=\"http://www.w3.org/2000/svg\">\n");
    for( const DataEntry & entry : dataset ) {
        std::printf(
            " <circle "
            "r=\"%lf\" "
            "cx=\"%lf\" "
            "cy=\"%lf\" "
            "fill=\"%s\" />\n",
            command_line::radius,
            (entry.attribute(0) - min_x) * scale_x,
            command_line::height - (entry.attribute(1) - min_y) * scale_y,
            color_of(entry.category(0)).c_str()
        );
    }
    std::printf("</svg>\n");

    return 0;
}
