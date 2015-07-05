#include <stdexcept>
#include <cfloat>
#include "cv.h"
#include "pr/dendogram_node.h"
#include "pr/grid_generator.h"

namespace util {

DataEntry entryFromVec( const cv::Vec3b & vec ) {
    /* Conversions from integer types to floating point types are "narrowing",
     * so the compiler is required to issue an error
     * when such conversion is used within braces - such as initializer lists.
     * (Working Draft for the C++ Standard, N3936, paragraph 8.5.4/7)
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

cv::Mat_<double> matFromEntry( const DataEntry & entry ) {
    std::size_t size = entry.attribute_count();
    cv::Mat_<double> r( 1, size );
    for( unsigned i = 0; i < size; i++ )
        r(0, i) = entry.attribute(i);
    return r;
}

const std::vector< cv::Scalar > color_list{
    cv::Scalar( 0, 0, 255 ),
    cv::Scalar( 0, 255, 0 ),
    cv::Scalar( 255, 0, 0 ),
    cv::Scalar( 255, 0, 255 ),
    cv::Scalar( 0, 255, 255 ),
    cv::Scalar( 255, 255, 0 ),
    cv::Scalar( 0, 0, 0 ),
};

cv::Scalar category_color( std::string category ) {
    static std::map< std::string, unsigned > color_map;
    static int index = 0;

    static const unsigned limit = color_list.size();

    auto pair = color_map.insert( std::make_pair(category, index) );
    if( pair.second ) {
        if( index == limit )
            throw "Too much different categories.";
        ++index;
    }
    return color_list[pair.first->second];
};

void show_dataset(
    cv::Mat & output,
    const DataSet & input,
    int radius,
    double border
) {
    if( input.attribute_count() != 2 )
        throw std::out_of_range(
            "The input dataset must have exactly two attributes."
        );
    if( input.category_count() != 1 )
        throw std::out_of_range(
            "The input dataset must have exactly one category type."
        );

    output = cv::Scalar( 255, 255, 255 );

    auto pair = input.normalizing_factor( border );
    double min_x = pair.second[0], min_y = pair.second[1];
    double scale_x = pair.first[0] * output.cols;
    double scale_y = pair.first[1] * output.rows;

    for( const DataEntry & e : input ) {
        int x = (e.attribute(0) - min_x) * scale_x;
        int y = (e.attribute(1) - min_y) * scale_y;
        auto color = category_color( e.category(0) );
        cv::circle( output, cv::Point(x, output.rows - y - 1), 0, color, radius);
    }
}

void influence_areas( cv::Mat & img, const NearestNeighbor & nn, double border ) {
    if( nn.dataset().attribute_count() != 2 )
        throw std::out_of_range(
            "The input dataset must have exactly two attributes."
        );
    if( nn.dataset().category_count() != 1 )
        throw std::out_of_range(
            "The input dataset must have exactly one category type."
        );

    GridGenerator grid;
    grid.expand( border );
    grid.density( std::vector<unsigned>{(unsigned) img.cols, (unsigned) img.rows} );
    grid.calibrate( nn.dataset() );

    for( int i = 0; i < img.rows; i++ )
        for( int j = 0; j < img.cols; j++ ) {
            DataEntry data = grid( {(unsigned)i, (unsigned)j} );
            std::string category = *nn.classify(data).begin();
            auto color = util::category_color(category);
            img.at<cv::Vec3b>(img.rows - j - 1, i) =
                cv::Vec3b( color[0], color[1], color[2] );
        }
}

void print_dendogram( cv::Mat & output, const DendogramNode & input ) {
    if( input.leaf() )
        return;

    int width = output.size().width;
    int height = output.size().height;

    if( input.linkage_distance() < DBL_EPSILON ) {
        /* This should only happen in the lower levels.
         * In this situation, width is most likely 0,
         * so we simply return.
         *
         * Width will only be nonzero if this is the "top" call,
         * so we will just draw some lines to give the impression
         * of all them joining in the same level.
         */
        if( width != 0 && height != 0 ) {
            for( int i = 0; i < input.size(); i++ ) {
                int draw_height = (2.0 * i + 1)/2 / input.size() * height;
                cv::line(
                    output,
                    cv::Point(0, draw_height),
                    cv::Point(width-1, draw_height),
                    cv::Scalar(0,0,0)
                );
            }
            int lowest_height = 0.5/input.size() * height;
            int highest_height = (input.size() - 0.5)/input.size() * height;
            cv::line(
                output,
                cv::Point(width-1, lowest_height),
                cv::Point(width-1, highest_height),
                cv::Scalar(0,0,0)
            );
        }
        return;
    }

    int middle = (double) input.left().size() / input.size() * height;
    int upper_limit = (double) input.left().linkage_distance() /
        input.linkage_distance() * width;
    int lower_limit = (double) input.right().linkage_distance() /
        input.linkage_distance() * width;

    cv::Mat upper_img = output( cv::Range(0, middle-1), cv::Range(0, upper_limit) );
    cv::Mat lower_img = output( cv::Range(middle, height-1), cv::Range(0, lower_limit) );
    print_dendogram( upper_img, input.left() );
    print_dendogram( lower_img, input.right() );

    int upper_middle = middle/2;
    int lower_middle = (middle + height)/2;
    cv::line(
        output,
        cv::Point(upper_limit, upper_middle),
        cv::Point(width, upper_middle),
        cv::Scalar(0,0,0)
    );
    cv::line(
        output,
        cv::Point(lower_limit, lower_middle),
        cv::Point(width, lower_middle),
        cv::Scalar(0,0,0)
    );
    cv::line(
        output,
        cv::Point(width-1, upper_middle),
        cv::Point(width-1, lower_middle),
        cv::Scalar(0,0,0)
    );
}

} // namespace util
