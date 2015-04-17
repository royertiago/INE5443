namespace command_line {
    const char help_message[] =
"%s [options] <image>\n"
"Given a set of pixel coordinates in stdin for the image,\n"
"this program will classify every other pixel of the image\n"
"against that set, according to the Mahalanobis distance.\n"
"\n"
"The \"classification\" indicates how close to the set of points\n"
"is any given pixel. There is no real classification\n"
"since there is only one category.\n"
"\n"
"Options:\n"
"--output <file>\n"
"    Write the generated image to <file>.\n"
"    No writing is done by default.\n"
"\n"
"--mahalanobis\n"
"    Uses the Mahalanobis distance as similarity metric.\n"
"    This is the default.\n"
"\n"
"--manhattan\n"
"--hamming\n"
"    Chooses Hamming/Manhattan distance as similarity metric.\n"
"\n"
"--euclidean\n"
"    Chooses euclidean distance as similarity metric.\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line

#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cmdline/args.h"
#include "util/csv.h"
#include "util/cv.h"
#include "pr/classifier.h"
#include "pr/grid_generator.h"
#include "pr/mahalanobis.h"
#include "pr/p_norm.h"

namespace command_line {
    std::string output;
    std::string image;

    bool hamming = false;
    bool euclidean = false;

    void parse( cmdline::args&& args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--output" ) {
                output = arg;
                continue;
            }
            if( arg == "--mahalanobis" ) {
                hamming = false;
                euclidean = false;
                continue;
            }
            if( arg == "--manhattan" || arg == "--hamming" ) {
                hamming = true;
                euclidean = false;
                continue;
            }
            if( arg == "--euclidean" ) {
                hamming = false;
                euclidean = true;
                continue;
            }
            if( arg == "--help" ) {
                std::printf( help_message, args.program_name().c_str() );
                std::exit(0);
            }
            if( image != "" ) {
                std::fprintf( stderr, "Image specified twice, as '%s' and '%s'\n",
                    image.c_str(), arg.c_str()
                );
                std::exit(1);
            }
            image = arg;
        }
        if( image == "" ) {
            std::fprintf( stderr, "Missing image source in command-line.\n" );
            std::exit(1);
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    auto data = util::parse_csv( stdin );
    cv::Mat img = cv::imread( command_line::image );

    DataSet dataset( 3, 0 );
    for( unsigned i = 0; i < data.size(); ++i )
        /* opencv's dimension access order is row/column.
         * The output of pixel_chooser is the opposite.
         */
        dataset.push_back( util::entryFromVec(
            img.at<cv::Vec3b>(data[i][1], data[i][0])
        ));

    std::unique_ptr< DistanceCalculator > distance_ptr;
    if( command_line::euclidean )
        distance_ptr = std::make_unique<EuclideanDistance>(0);
    else if( command_line::hamming )
        distance_ptr = std::make_unique<ManhattanDistance>(0);
    else
        distance_ptr = std::make_unique<MahalanobisDistance>();
    DistanceCalculator & distance = *distance_ptr;
    distance.calibrate(dataset);

    DataEntry mean = dataset.mean();

    auto it = img.begin<cv::Vec3b>(), end = img.end<cv::Vec3b>();
    for( ; it != end; ++it ) {
        DataEntry entry = util::entryFromVec( *it );

        double dist = distance( mean, entry );
        double sim; // similarity
        if( dist > 1 )
            sim = 0;
        else
            sim = 1 - dist;

        *it = cv::Vec3b(255 * sim, 255 * sim, 255 * sim);
    }

    if( command_line::output != "" ) {
        if( !cv::imwrite( command_line::output, img ) )
            std::fprintf( stderr, "Error writing image to %s\n",
                command_line::output.c_str()
            );
    }

    cv::namedWindow( "Mahalanobis", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Mahalanobis", img );
    cv::waitKey();

    return 0;
}
