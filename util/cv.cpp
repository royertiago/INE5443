#include "cv.h"

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


} // namespace util
