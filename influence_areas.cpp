namespace command_line {
    const char help_message[] =
" [options]\n"
"Generates an image that contains the influence areas\n"
"of each category of the dataset.\n"
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
"--noise <N>\n"
"    Enable generation of some noise in the dataset.\n"
"    This command generates N noise points.\n"
"\n"
"--noise-seed <N>\n"
"    Choose N as the seed for the noise generation algorithm.\n"
"    The noise is generated before shuffling.\n"
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
#include "cmdline/args.hpp"
#include "pr/classifier.h"
#include "util/cv.h"

namespace command_line {
    std::string output_file_name = "output.png";
    int width = 500;
    int height = 500;
    double expand = 0.05;

    int noise = 0;
    long long unsigned noise_seed;
    bool noise_seed_set = false;
    double noise_expand = 0.1;

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

    if( command_line::noise != 0 ) {
        if( command_line::noise_seed_set ) {
            classifier.edit_dataset().noise(
                command_line::noise,
                command_line::noise_expand,
                command_line::noise_seed
            );
        }
        else {
            long long unsigned seed = classifier.edit_dataset().noise(
                command_line::noise,
                command_line::noise_expand
            );
            std::printf( "Noise seed: %llu\n", seed );
        }
    }

    cv::Mat img( height, width, CV_8UC3, cv::Scalar(255, 255, 255) );
    util::influence_areas( img, classifier, expand );

    if( !cv::imwrite( output_file_name, img ) ) {
        std::cerr << "Error writing image to " << output_file_name << '\n';
        return 3;
    }

    return 0;
}
