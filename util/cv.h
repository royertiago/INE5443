#ifndef UTIL_CV_H
#define UTIL_CV_H

/* Utility functions to convert between opencv
 * and our internal representation.
 */

#include <opencv2/core/core.hpp>
#include "pr/data_entry.h"

namespace util {

    /* Retruns a DataEntry whose values are the given vector.
     */
    DataEntry entryFromVec( const cv::Vec3b & vec );

    /* Returns a matrix with one column and several rows
     * with the attributes of the entry. */
    cv::Mat_<double> matFromEntry( const DataEntry & );

}
#endif // UTIL_CV_H
