#include <cstdio>
#include <cstring>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>

std::vector<std::vector<bool>> chosen;
cv::Mat img;
const char * window = "Choose the points";

int main( int argc, char ** argv ) {
    if( argc != 2 || strcmp(argv[1], "--help") == 0 ) {
        std::printf(
            "Usage: %s <image>\n"
            "Displays the image on the screen and lets the user select pixels,\n"
            "using the mouse.\n"
            "The list of selected pixels is written, in column,row format, to stdout.\n"
            "Duplicated pixels are NOT written twice to stdout.\n"
            ,
            argv[0]
        );
        return 0;
    }

    img = cv::imread( argv[1], CV_LOAD_IMAGE_COLOR );
    if( img.empty() ) {
        std::fprintf( stderr, "Error opening image.\n" );
        return 1;
    }

    chosen = std::vector<std::vector<bool>>(
        img.cols,
        std::vector<bool>(img.rows, false)
    );

    cv::namedWindow( window, cv::WINDOW_AUTOSIZE );
    cv::setMouseCallback( window,
        []( int event, int x, int y, int /*flags*/, void * /*user_data*/ ) {
            if( event == cv::EVENT_LBUTTONDOWN ) {
                if( !chosen[x][y] )
                    std::printf( "%i,%i\n", x, y );
                chosen[x][y] = true;
                cv::circle( img, cv::Point(x, y), 0, cv::Scalar(0, 0, 0), 6);
                cv::imshow( window, img );
            }
        },
        nullptr
    );
    cv::imshow( window, img );

    // No idea why I cannot use 'while( (char) cv::waitKey() != 0 )'...
    while( true ) {
        char c = cv::waitKey();
        if( c != 0 ) break;
    }

    return 0;
}
