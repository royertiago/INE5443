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
"--seed <N>\n"
"    Choose <N> as the seed for the shuffling algorithm.\n"
"    Default: Generate a seed and print to stdout.\n"
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
    bool seed_set = false;
    long long unsigned seed;

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
            if( arg == "--seed" ) {
                args >> seed;
                seed_set = true;
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
    cv::Mat img( height, 3 * width, CV_8UC3 );

    cv::Mat left, middle, right;
    left = img(cv::Range::all(), cv::Range(0, width) );
    middle = img(cv::Range::all(), cv::Range(width, 2*width) );
    right = img(cv::Range::all(), cv::Range(2*width, 3*width) );
    cv::namedWindow( "IBL", cv::WINDOW_AUTOSIZE );

    util::show_dataset( left, dataset );
    cv::imshow( "IBL", img );

    if( command_line::shuffle ) {
        if( command_line::seed_set )
            dataset.shuffle( command_line::seed );
        else {
            auto seed = dataset.shuffle();
            std::cout << "Seed: " << seed << std::endl;
        }
    }
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
