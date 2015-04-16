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

cv::Vec3b category_color( std::string category ) {
    static std::map<std::string, const unsigned *> color_map;
    static int index = 0;

    static const unsigned red[] = {255, 0, 0};
    static const unsigned green[] = {0, 255, 0};
    static const unsigned blue[] = {0, 0, 255};
    static const unsigned yellow[] = {255, 0, 255};
    static const unsigned magenta[] = {255, 255, 0};
    static const unsigned cyan[] = {0, 255, 255};
    static const unsigned black[] = {0, 0, 0};
    static const unsigned * colors[] =
    {red, green, blue, yellow, magenta, cyan, black};
    static constexpr unsigned limit = sizeof(colors)/sizeof(colors[0]);

    auto pair = color_map.insert( std::make_pair(category, colors[index]) );
    if( pair.second ) {
        if( index == limit )
            throw "Too much different categories.";
        ++index;
    }

    const unsigned * c = pair.first->second;
    return cv::Vec3b( c[2], c[1], c[0] );
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

    double max_x = -DBL_MAX, max_y = -DBL_MAX;
    double min_x = DBL_MAX, min_y = DBL_MAX;
    for( const DataEntry & e : input ) {
        max_x = std::max( max_x, e.attribute(0) );
        min_x = std::min( min_x, e.attribute(0) );
        max_y = std::max( max_y, e.attribute(1) );
        min_y = std::min( min_y, e.attribute(1) );
    }

    double scale_x = output.cols / (max_x - min_x);
    double scale_y = output.rows / (max_y - min_y);
    double shift_x = output.cols * border;
    double shift_y = output.rows * border;
    scale_x /= (1 + 2*border);
    scale_y /= (1 + 2*border);

    for( const DataEntry & e : input ) {
        int x = (e.attribute(0) - min_x) * scale_x + shift_x;
        int y = (e.attribute(1) - min_y) * scale_y + shift_y;
        auto color = category_color( e.category(0) );
        cv::circle( output, cv::Point(x, y), 0, cv::Scalar(color), radius);
    }
}

void influence_areas( cv::Mat & img, const Classifier & input, double border ) {
    if( input.dataset().attribute_count() != 2 )
        throw std::out_of_range(
            "The input dataset must have exactly two attributes."
        );
    if( input.dataset().category_count() != 1 )
        throw std::out_of_range(
            "The input dataset must have exactly one category type."
        );

    GridGenerator grid;
    grid.expand( border );
    grid.density( std::vector<unsigned>{(unsigned) img.cols, (unsigned) img.rows} );
    grid.calibrate( input.dataset() );

    for( int i = 0; i < img.rows; i++ )
        for( int j = 0; j < img.cols; j++ ) {
            DataEntry data = grid( {(unsigned)i, (unsigned)j} );
            std::string category = *input.classify(data).begin();
            img.at<cv::Vec3b>(img.rows - j - 1, i) = util::category_color(category);
        }
}

} // namespace util
