#include <stdexcept>
#include <cfloat>
#include "cv.h"
#include "pr/grid_generator.h"

namespace util {

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

} // namespace util
