#include <cstdio>
#include <cstring>
#include <vector>
#include <opencv2/highgui/highgui.hpp>

std::vector<std::vector<bool>> chosen;
cv::Mat img;

int main( int argc, char ** argv ) {
    if( argc != 2 || strcmp(argv[1], "--help") == 0 ) {
        std::printf(
            "Usage: %s <image>\n"
            "Displays the image on the screen and lets the user select pixels,\n"
            "using the mouse.\n"
            "The list of selected pixels is written, in CSV format, to stdout.\n"
            "Duplicated pixels are NOT written twice to stdout.\n"
            ,
            argv[0]
        );
        return 0;
    }

    img = cv::imread( argv[1] );
    if( img.empty() ) {
        std::fprintf( stderr, "Error opening image.\n" );
        return 1;
    }

    chosen = std::vector<std::vector<bool>>(
        img.cols,
        std::vector<bool>(img.rows, false)
    );

    cv::namedWindow( "Choose the points", cv::WINDOW_AUTOSIZE );
    cv::setMouseCallback( "Choose the points",
        []( int event, int x, int y, int flags, void * /*user_data*/ ) {
            if( event == cv::EVENT_LBUTTONDOWN ) {
                if( !chosen[x][y] )
                    std::printf( "%i,%i\n", x, y );
                chosen[x][y] = true;
            }
        },
        nullptr
    );
    cv::imshow( "Choose the points", img );
    cv::waitKey( 0 );

    return 0;
}
