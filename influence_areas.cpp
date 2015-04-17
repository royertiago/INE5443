namespace command_line {
    const char help_message[] =
" [options]\n"
"Generates an image that contains the influence areas\n"
"of each category sy the dataset.\n"
"Input is read on stdin.\n"
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
"--normalize\n"
"--no-normalize\n"
"--normalize-tolerance <F>\n"
;
} // namespace command_line

#include <cfloat>
#include <cstdlib>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cmdline/args.h"
#include "pr/classifier.h"
#include "util/cv.h"

namespace command_line {
    std::string output_file_name = "output.png";
    int width = 500;
    int height = 500;
    double expand = 0.05;

    /* Arguments that will be passed to the classifier. */
    cmdline::args subargs;

    void parse( cmdline::args&& args ) {
        subargs.program_name(args.program_name());
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--output" ) {
                output_file_name = args.next();
                continue;
            }
            if( arg == "--width" ) {
                args >> width;
                continue;
            }
            if( arg == "--height" ) {
                args >> height;
                continue;
            }
            if( arg == "--expand" ) {
                args >> expand;
                continue;
            }
            if( arg == "--manhattan"
             || arg == "--hamming"
             || arg == "--euclidean"
             || arg == "--normalize"
             || arg == "--no-normalize" ) {
                subargs.push_back( arg );
                continue;
            }
            if( arg == "--neighbors" || arg == "--normalize-tolerance" ) {
                subargs.push_back( arg );
                subargs.push_back( args.next() );
                continue;
            }
            if( arg == "--help" ) {
                std::cout << args.program_name() << help_message;
                std::exit(0);
            }
            std::cerr << args.program_name() << ": Unknown option " << arg << '\n';
            std::exit(1);
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    using namespace command_line;

    command_line::parse( cmdline::args(argc,argv) );

    auto ptr = generate_classifier( std::move(command_line::subargs) );
    NearestNeighbor & classifier = *ptr;

    if( classifier.dataset().attribute_count() != 2 ) {
        std::cerr << "The database must have exactly two atributes.\n";
        std::exit( 2 );
    }

    if( classifier.dataset().category_count() != 1 ) {
        std::cerr << "The database must have exactly one category.\n";
        std::exit( 2 );
    }

    cv::Mat img( height, width, CV_8UC3, cv::Scalar(255, 255, 255) );
    util::influence_areas( img, classifier, expand );

    if( !cv::imwrite( output_file_name, img ) ) {
        std::cerr << "Error writing image to " << output_file_name << '\n';
        return 3;
    }

    return 0;
}
