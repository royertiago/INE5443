namespace command_line {
    const char help_message[] =
"%s [options]\n"
"Generates a svg file with the influence areas of the given dataset.\n"
"Input is read on stdin, output is written in stdout.\n"
"\n"
"Options:\n"
"--output <file>\n"
"    Write the output to <file>.\n"
"    Default: output.png\n"
"\n"
"--width <N>\n"
"--height <N>\n"
"    Choose the dimensions of the image.\n"
"    Default value: 500 (for both).\n"
"\n"
"--expand <F>\n"
"    Percentage of the dataset extension that should be used as border.\n"
"    Default value: 0.05.\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
"\n"
"Classifier options:\n"
"These options are passed directly to the classifier.\n"
"Run .classify --help for more information.\n"
"--manhattan\n"
"--hamming\n"
"--euclidean\n"
"--neighbors <N>\n"
"--normalize-tolerance <F>\n"
;
} // namespace command_line

#include <getopt.h>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "pr/classifier.h"
#include "pr/grid_generator.h"

namespace command_line {
    const char * output = "output.png";
    int width = 500;
    int height = 500;
    double expand = 0.05;

    /* Arguments that will be passed to the classifier. */
    std::vector< char * > args;

    /* We will use freopen to reset stdin and stdout
     * in the presence of --input or --output options. */
    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"output", required_argument, 0, 'o'},
            {"width", required_argument, 0, 'w'},
            {"height", required_argument, 0, 'a'},
            {"expand", required_argument, 0, 'e'},

            {"manhattan", no_argument, 0, 'm'},
            {"hamming", no_argument, 0, 'm'},
            {"euclidean", no_argument, 0, 'c'},
            {"neighbors", required_argument, 0, 'n'},
            {"normalize-tolerance", required_argument, 0, 't'},

            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        args.push_back( argv[0] );
        while( (opt = getopt_long( argc, argv, "men:t:d:h",
                    options, &dummy_option_index
                )) != -1 ) {
            switch( opt ) {
                case 'o':
                    output = optarg;
                    break;

                case 'w':
                    if( std::sscanf( optarg, "%d", &width ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( width <= 0 ) {
                        std::fprintf( stderr, "Width must be positive.\n" );
                        std::exit(1);
                    }
                    break;

                case 'a':
                    if( std::sscanf( optarg, "%d", &height ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( height <= 0 ) {
                        std::fprintf( stderr, "Height must be positive.\n" );
                        std::exit(1);
                    }
                    break;

                case 'e':
                    if( std::sscanf( optarg, "%lf", &expand ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( expand < 0 ) {
                        std::fprintf( stderr, "Expansion must be non-negative.\n" );
                        std::exit(1);
                    }
                    break;

                case 'm':
                case 'c':
                    args.push_back( argv[optind-1] );
                    break;
                case 'n':
                case 't':
                    args.push_back( argv[optind-2] );
                    args.push_back( optarg );
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
        args.push_back( nullptr );
    } // void parse(int, char**)
} // namespace command_line

int main( int argc, char ** argv ) {
    using namespace command_line;

    command_line::parse( argc, argv );

    Classifier classifier( args.size() - 1, args.data() );

    if( classifier.dataset().attribute_count() != 2 ) {
        std::fprintf( stderr, "The database must have exactly two atributes.\n" );
        std::exit( 2 );
    }

    if( classifier.dataset().category_count() != 1 ) {
        std::fprintf( stderr, "The database must have exactly one category.\n" );
        std::exit( 2 );
    }

    GridGenerator grid;
    grid.expand( expand );
    grid.density( std::vector<unsigned>{(unsigned) width, (unsigned) height} );
    grid.calibrate( classifier.dataset() );

    auto color = [
        color_map = std::map<std::string, const unsigned *>(),
        index = 0
        ]
        ( std::string category ) mutable
        -> const unsigned *
        {
            static const unsigned red[] = {255, 0, 0};
            static const unsigned green[] = {0, 255, 0};
            static const unsigned blue[] = {0, 0, 255};
            static const unsigned yellow[] = {255, 0, 255};
            static const unsigned magenta[] = {255, 255, 0};
            static const unsigned cyan[] = {0, 255, 255};
            static const unsigned black[] = {0, 0, 0};
            static const unsigned * colors[] =
            {red, green, blue, yellow, magenta, cyan, black};
            static constexpr unsigned limit = sizeof(colors)/sizeof(colors[0]);

            auto pair = color_map.insert( std::make_pair(category, colors[index]) );
            if( pair.second ) {
                if( index == limit )
                    throw "Too much different categories.";
                ++index;
            }
            return pair.first->second;
        };

    cv::Mat img( height, width, CV_8UC3, cv::Scalar(255, 255, 255) );
    for( int i = 0; i < width; i++ )
        for( int j = 0; j < height; j++ ) {
            DataEntry data = grid( {(unsigned)i, (unsigned)j} );
            std::string category = *classifier.classify(data).begin();
            const unsigned * c = color(category);
            img.at<cv::Vec3b>(height - j - 1, i) = cv::Vec3b(c[0], c[1], c[2]);
        }

    if( !cv::imwrite( output, img ) ) {
        std::fprintf( stderr, "Error writing image to %s\n", output );
        return 3;
    }

    return 0;
}
