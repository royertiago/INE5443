namespace command_line {
    const char help_message[] =
"%s [options]\n"
"Learn the dataset read in stdin through IBL and show\n"
"the original dataset, the remaining conceptual descriptor\n"
"and the influence areas of the conceptual descriptor.\n"
"\n"
"Options:\n"
"--width <N>\n"
"--height <N>\n"
"    Sets the width/height of each image.\n"
"    Default: 300.\n"
"\n"
"--ibl <N>\n"
"--IBL <N>\n"
"    Chooses the IBL algorithm.\n"
"    Default: 1.\n"
"\n"
"--shuffle\n"
"    Shuffle the input dataset before giving it to the IBL algorithm.\n"
"    Default: no shuffle.\n"
"\n"
"--shuffle-seed <N>\n"
"    Choose <N> as the seed for the shuffling algorithm.\n"
"    Implies --shuffle.\n"
"    Default: Generate a seed and print to stdout.\n"
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
"    Display this help and quit.\n"
;
} // namespace command_line

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "pr/ibl.h"
#include "util/cv.h"

namespace command_line {
    int width = 300;
    int height = 300;
    int ibl = 1;
    bool shuffle = false;
    bool shuffle_seed_set = false;
    long long unsigned shuffle_seed;

    int noise = 0;
    long long unsigned noise_seed;
    bool noise_seed_set = false;
    double noise_expand = 0.1;

    void parse( cmdline::args&& args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--height" ) {
                args.range(1) >> height;
                continue;
            }
            if( arg == "--width" ) {
                args.range(1) >> width;
                continue;
            }
            if( arg == "--ibl" || arg == "--IBL" ) {
                args.range(1, 2) >> ibl;
                continue;
            }
            if( arg == "--shuffle" ) {
                shuffle = true;
                continue;
            }
            if( arg == "--shuffle-seed" ) {
                args >> shuffle_seed;
                shuffle = true;
                shuffle_seed_set = true;
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
            std::printf( "Unknown argument %s.\n", arg.c_str() );
            std::exit(1);
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    DataSet dataset = DataSet::parse( stdin );

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

    if( command_line::shuffle ) {
        if( command_line::shuffle_seed_set )
            dataset.shuffle( command_line::shuffle_seed );
        else {
            auto seed = dataset.shuffle();
            std::cout << "Shuffle seed: " << seed << std::endl;
        }
    }

    std::unique_ptr<ibl> ibl_ptr;
    switch( command_line::ibl ) {
        case 1:
            ibl_ptr = std::make_unique<ibl1>();
            break;
        case 2:
            ibl_ptr = std::make_unique<ibl2>();
            break;
    }
    ibl & ibl = *ibl_ptr;

    using command_line::width;
    using command_line::height;
    /* We add two pixels in the width so that the final image
     * will have two black lines, of one pixel each, separating
     * each of its "panels". */
    cv::Mat img( height, 3 * width + 2, CV_8UC3, cv::Scalar(0, 0, 0) );

    cv::Mat left, middle, right;
    left = img(cv::Range::all(), cv::Range(0, width) );
    middle = img(cv::Range::all(), cv::Range(width + 1, 2*width + 1) );
    right = img(cv::Range::all(), cv::Range(2*width + 2, 3*width + 2) );
    cv::namedWindow( "IBL", cv::WINDOW_AUTOSIZE );

    util::show_dataset( left, dataset );
    cv::imshow( "IBL", img );

    ibl.train( dataset );
    std::cout << "Hits: " << ibl.hit_count()
        << " - Misses: " << ibl.miss_count() << "\n";

    util::show_dataset( middle, ibl.conceptual_descriptor() );
    cv::imshow( "IBL", img );
    cv::waitKey(10);

    util::influence_areas( right, ibl.nearest_neighbor(), 0.1 );
    cv::imshow( "IBL", img );
    cv::waitKey();

    return 0;
}
