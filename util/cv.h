#ifndef UTIL_CV_H
#define UTIL_CV_H

/* Utility functions to convert between opencv
 * and our internal representation.
 */

#include <opencv2/core/core.hpp>
#include "pr/data_entry.h"
#include "pr/data_set.h"

namespace util {

    /* Retruns a DataEntry whose values are the given vector.
     */
    DataEntry entryFromVec( const cv::Vec3b & vec );

    /* Returns a matrix with one column and several rows
     * with the attributes of the entry. */
    cv::Mat_<double> matFromEntry( const DataEntry & );

    /* Returns a Vec3b with a color according to the category.
     *
     * Each category will be assigned a different color.
     * The assignment is done on demand, and there is only seven avaliable colors.
     */
    cv::Vec3b category_color( std::string category );

    /* "Prints" the dataset in the cv::Mat.
     * Each point of the dataset become a circle in the image, with the chosen radius.
     * The background is white.
     *
     * The border is a fraction of the actual dataset.
     *
     * This function uses util::category_color to decide the color of each point.
     */
    void show_dataset(
        cv::Mat & output,
        const DataSet & intput,
        int radius = 6,
        double border = 0.1
    );
} // namespace util

#endif // UTIL_CV_H
