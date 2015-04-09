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
"--help\n"
"    Display this help and exit.\n"
;
} // namespace command_line

#include <getopt.h>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "util/csv.h"
#include "pr/classifier.h"
#include "pr/grid_generator.h"
#include "pr/mahalanobis.h"

namespace command_line {
    const char * output = nullptr;
    const char * image = nullptr;

    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"output", required_argument, 0, 'o'},
            {"help", no_argument, 0, 'h'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        while( (opt = getopt_long( argc, argv, "o:h",
                    options, &dummy_option_index
                )) != -1 ) {
            switch( opt ) {
                case 'o':
                    output = optarg;
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

        if( optind >= argc ) {
            std::fprintf( stderr, "Missing image source in command-line.\n" );
            std::exit(1);
        }

        image = argv[optind++];

        if( optind < argc ) {
            std::fprintf( stderr, "Spurious command-line option %s\n", argv[optind] );
            std::exit(1);
        }
    } // void parse(int, char**)
} // namespace command_line

DataEntry entryFromVec( const cv::Vec3b & vec ) {
    /* As the paragraph 8.5.4/7 of the C++11 standard,
     * conversions from integer types to floating point types are "narrowing",
     * so the compiler is obligate to issue an error
     * when such conversion is used within braces - such as initializer lists.
     *
     * Here, we silence this error.
     */
    return DataEntry({
        static_cast<double>( vec.val[0] ),
        static_cast<double>( vec.val[1] ),
        static_cast<double>( vec.val[2] ),
    },
    {} );
}

int main( int argc, char ** argv ) {
    command_line::parse( argc, argv );

    auto data = util::parse_csv( stdin );
    cv::Mat img = cv::imread( command_line::image );

    DataSet dataset( 3, 0 );
    for( unsigned i = 0; i < data.size(); ++i )
        /* opencv's dimension access order is row/column.
         * The output of pixel_chooser is the opposite.
         */
        dataset.push_back( entryFromVec(img.at<cv::Vec3b>(data[i][1], data[i][0])) );

    MahalanobisDistance distance;
    distance.calibrate(dataset);
    DataEntry mean = dataset.mean();

    auto it = img.begin<cv::Vec3b>(), end = img.end<cv::Vec3b>();
    for( ; it != end; ++it ) {
        DataEntry entry = entryFromVec( *it );

        double dist = distance( mean, entry );
        double sim; // similarity
        if( dist > 1 )
            sim = 0;
        else
            sim = 1 - dist;

        *it = cv::Vec3b(255 * sim, 255 * sim, 255 * sim);
    }

    if( command_line::output != nullptr ) {
        if( !cv::imwrite( command_line::output, img ) )
            std::fprintf( stderr, "Error writing image to %s\n", command_line::output );
    }

    cv::namedWindow( "Mahalanobis", CV_WINDOW_AUTOSIZE );
    cv::imshow( "Mahalanobis", img );
    cv::waitKey();

    return 0;
}
