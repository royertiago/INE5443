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

} // namespace util
