#include <algorithm>
#include <stdexcept>
#include "cv.h"
#include "pr/dendogram_node.h"
#include "pr/grid_generator.h"

#include <stdio.h>

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

int print_dendogram( cv::Mat & output, const DendogramNode & input ) {
    int width = output.size().width;
    int height = output.size().height;

    if( input.leaf() )
        return height/2;

    int middle = (double) input.left().size() / input.size() * height;
    int upper_limit = (double) input.left().linkage_distance() /
        input.linkage_distance() * width;
    int lower_limit = (double) input.right().linkage_distance() /
        input.linkage_distance() * width;

    // This guarantees each image will be at least one pixel wide.
    upper_limit = std::max( upper_limit, 1 );
    lower_limit = std::max( lower_limit, 1 );

    cv::Mat upper_img = output( cv::Range(0, middle), cv::Range(0, upper_limit) );
    cv::Mat lower_img = output( cv::Range(middle, height), cv::Range(0, lower_limit) );

    /* Cartesian plane in ASCII art outlining the variables:
     *
     *                          (upper_limit, 0)
     *     (0, 0) ________________________________________________(width, 0)
     *     |                      |                               |
     *     |                      |                               |
     *     |                      |                               |
     *     |                      |                               |
     *     |     [upper_img]      |(upper_limit, upper_middle)    |
     *     |                      |                               |
     *     |                      |                               |
     *     |                      |                               |
     *     (0, middle)____________|_____________                  |
     *     |                                    |                 |
     *     |                                    |                 |
     *     |                                    |                 |
     *     |                                    |                 |
     *     |                                    |                 |
     *     |            [lower_img]             |(lower_limit, lower_middle)
     *     |                                    |                 |
     *     |                                    |                 |
     *     |                                    |                 |
     *     |                                    |                 |
     *     (0, height)__________________________|_________________(width, height)
     *                                          (lower_limit, height)
     *
     * Using the power of recursion,
     * we will somehow draw the left and right childs
     * in [upper_img] and [lower_img], respectively,
     * and connect the points (upper_limit, upper_middle)
     * and (lower_limit, lower_middle) to the right border,
     * and draw a line between their connection points
     * --- this way both subtrees will look connected.
     */

    int upper_middle = print_dendogram( upper_img, input.left() );
    int lower_middle = print_dendogram( lower_img, input.right() ) + middle;

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

    return (upper_middle + lower_middle) / 2;
}

int print_named_dendogram( cv::Mat & output, const DendogramNode & input ) {
    constexpr int fontFace = cv::FONT_HERSHEY_PLAIN;
    constexpr int fontScale = 1;
    constexpr int fontThickness = 1;
    constexpr int border = 5; // Five pixels of border around the text.

    int baseline; // We ignore this number.

    int widest_text = 0;
    // First, find widest text
    for( const auto & entry : input ) {
        cv::Size size = cv::getTextSize(
            entry.name(), fontFace, fontScale, fontThickness, &baseline
        );
        widest_text = widest_text > size.width ? widest_text : size.width;
    }

    // Now, print all texts
    int i = 0; // This will be used to define the text height
    for( const auto & entry : input ) {
        cv::Size size = cv::getTextSize(
            entry.name(), fontFace, fontScale, fontThickness, &baseline
        );
        int center = (2.0 * i + 1)/(2*input.size()) * output.size().height;
        int bottom = center + size.height/2;
        int left = widest_text - size.width + border;
        cv::putText(
            output, entry.name(), cv::Point( left, bottom ),
            fontFace, fontScale, cv::Scalar(0,0,0)
        );
        i++;
    }

    // And finally, print the dendogram
    cv::Mat remaining_image = output(
        cv::Range::all(),
        cv::Range( widest_text + 2*border, output.size().width )
    );
    print_dendogram( remaining_image, input );

    return 2 * border + widest_text;
}

} // namespace util
