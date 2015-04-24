namespace command_line {
    const char help_message[] =
"%s <image>\n"
"Displays the image on the screen and lets the user select pixels,\n"
"using the mouse.\n"
"The list of selected pixels is written, in column,row format, to stdout.\n"
"Duplicated pixels are NOT written twice to stdout.\n"
"\n"
"Options:\n"
"--dataset\n"
"    Output a dataset instead of a list of pixels.\n"
"    You can use different categories for each pixel by pressing spacebar.\n"
"    Output is written to stdout.\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line
#include <cstdio>
#include <cstring>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
#include "cmdline/args.h"
#include "pr/data_set.h"
#include "util/cv.h"

namespace command_line {
    bool generate_dataset = false;
    std::string image_name = "";

    void parse( cmdline::args && args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--dataset" ) {
                generate_dataset = true;
                continue;
            }
            if( arg == "--help" ) {
                std::printf( help_message, args.program_name().c_str() );
                std::exit(0);
            }
            if( image_name != "" ) {
                std::fprintf( stderr, "Two image supplied: '%s' and '%s'\n",
                    image_name.c_str(), arg.c_str()
                );
                std::exit(1);
            }
            image_name = arg;
        }
    }
} // namespace command_line

std::vector<std::vector<bool>> chosen;
DataSet dataset(
    std::vector<std::string>{"X position", "Y position"},
    std::vector<std::string>{"Category"},
    std::vector<DataEntry>{}
);
cv::Mat img;
const char * window = "Choose the points";
int color_index = 0;

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    img = cv::imread( command_line::image_name, CV_LOAD_IMAGE_COLOR );
    if( img.empty() ) {
        std::fprintf( stderr, "Error opening image.\n" );
        return 1;
    }

    chosen = std::vector<std::vector<bool>>(
        img.cols,
        std::vector<bool>(img.rows, false)
    );

    static auto new_point = []( int x, int y ) {
        if( command_line::generate_dataset ) {
            std::string category = "A";
            category[0] += color_index;
            dataset.push_back( DataEntry({(double)x, (double)y},{category}) );
        }
        else
            std::printf( "%i,%i\n", x, y );
    };

    cv::namedWindow( window, cv::WINDOW_AUTOSIZE );
    cv::setMouseCallback( window,
        []( int event, int x, int y, int /*flags*/, void * /*user_data*/ ) {
            if( event == cv::EVENT_LBUTTONDOWN ) {
                if( !chosen[x][y] )
                    new_point( x, y );
                chosen[x][y] = true;
                cv::circle( img, cv::Point(x, y), 0, util::color_list[color_index], 6);
                cv::imshow( window, img );
            }
        },
        nullptr
    );
    cv::imshow( window, img );

    // No idea why I cannot use 'while( (char) cv::waitKey() != 0 )'...
    while( true ) {
        char c = cv::waitKey();
        if( c == ' ' )
            color_index = (color_index + 1) % util::color_list.size();
        else if( c != 0 )
            break;
    }

    if( command_line::generate_dataset ) {
        dataset.write( stdout );
    }
    return 0;
}
