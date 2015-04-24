namespace command_line {
    const char help_message[] =
"%s <image>\n"
"Displays the image on the screen and lets the user select pixels,\n"
"using the mouse.\n"
"The list of selected pixels is written, in column,row format, to stdout.\n"
"Duplicated pixels are NOT written twice to stdout.\n"
"\n"
"Options:\n"
"--blank <width> <height>\n"
"   Instead of choosing pixels from an image, do so from a\n"
"   blank background, with the specified dimensions.\n"
"\n"
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

    bool blank_background = false;
    int blank_width, blank_height;

    void parse( cmdline::args && args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--dataset" ) {
                generate_dataset = true;
                continue;
            }
            if( arg == "--blank" ) {
                args.range( 1 ) >> blank_width;
                args.range( 1 ) >> blank_height;
                blank_background = true;
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
/* hover_img is the same as fixed_img,
 * but with a circle under the mouse,
 * to provide some visual feedback.
 */
cv::Mat fixed_img;
cv::Mat hover_img;
/* The mouse coordinates, mouse_x and mouse_y, are updated by a lambda function
 * called every time the mouse moves. This is used to update the screen
 * when the user press space, even if the user does not move the mouse right after.
 */
int mouse_x = 0, mouse_y = 0;
const char * window = "Choose the points";
int color_index = 0;

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    if( !command_line::blank_background ) {
        if( command_line::image_name == "" ) {
            std::fprintf( stderr, "Image not supplied nor --blank option used.\n" );
            return 1;
        }
        fixed_img = cv::imread( command_line::image_name, CV_LOAD_IMAGE_COLOR );
        if( fixed_img.empty() ) {
            std::fprintf( stderr, "Error opening image.\n" );
            return 1;
        }
    }
    else {
        fixed_img = cv::Mat(
            command_line::blank_width,
            command_line::blank_height,
            CV_8UC3,
            cv::Scalar(255, 255, 255)
        );
    }

    chosen = std::vector<std::vector<bool>>(
        fixed_img.cols,
        std::vector<bool>(fixed_img.rows, false)
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

    static auto show_hover = [](){
        hover_img = fixed_img.clone();
        cv::circle( hover_img, cv::Point(mouse_x, mouse_y), 0,
            util::color_list[color_index], 6 );
        cv::imshow( window, hover_img );
    };

    cv::namedWindow( window, cv::WINDOW_AUTOSIZE );
    cv::setMouseCallback( window,
        []( int event, int x, int y, int /*flags*/, void * /*user_data*/ ) {
            if( event == cv::EVENT_LBUTTONDOWN ) {
                if( !chosen[x][y] )
                    new_point( x, y );
                chosen[x][y] = true;
                fixed_img = hover_img.clone();
                cv::imshow( window, fixed_img );
            }
            if( event == cv::EVENT_MOUSEMOVE ) {
                mouse_x = x;
                mouse_y = y;
                show_hover();
            }
        },
        nullptr
    );
    cv::imshow( window, fixed_img );

    // No idea why I cannot use 'while( (char) cv::waitKey() != 0 )'...
    while( true ) {
        char c = cv::waitKey();
        if( c == ' ' ) {
            color_index = (color_index + 1) % util::color_list.size();
            show_hover();
        }
        else if( c != 0 )
            break;
    }

    if( command_line::generate_dataset ) {
        dataset.write( stdout );
    }
    return 0;
}
