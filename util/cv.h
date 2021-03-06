#ifndef UTIL_CV_H
#define UTIL_CV_H

/* Utility functions to convert between opencv
 * and our internal representation.
 */

#include <opencv2/core/core.hpp>
#include "pr/classifier.h"
#include "pr/data_entry.h"
#include "pr/data_set.h"
#include "pr/dendogram.h"

namespace util {

    /* Retruns a DataEntry whose values are the given vector.
     */
    DataEntry entryFromVec( const cv::Vec3b & vec );

    /* Returns a matrix with one column and several rows
     * with the attributes of the entry.
     */
    cv::Mat_<double> matFromEntry( const DataEntry & );

    /* Returns a Vec3b with a color according to the category.
     *
     * Each category will be assigned a different color.
     * The assignment is done on demand, and there is only seven avaliable colors.
     */
    cv::Scalar category_color( std::string category );

    /* List of colors avaliable through util::category_color.
     */
    extern const std::vector< cv::Scalar > color_list;

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

    /* Writes the influence areas of the given dataset to the cv::Mat.
     * The dataset must be encapsulated in a Classifier,
     * equiped with the metric that will be used to compute the influence areas.
     *
     * border is the percentage of the data set extremum points
     * that will be used to create a border.
     */
    void influence_areas(
        cv::Mat & output,
        const NearestNeighbor & input,
        double border
    );

    /* Draws the dendogram in the image.
     * The leaves will face left, and the root will face right.
     *
     * The leaves, from top to bottom, corresponds to
     * passing from begin() to end() of the given dendogram.
     *
     * No text is appended.
     *
     * Returns the height of the middle of the root node.
     */
    int print_dendogram(
        cv::Mat & output,
        const DendogramNode & input
    );

    /* Same as print_dendogram,
     * but writes the names of each node in the left of the dendogram.
     *
     * Returns the width of the area used for writing the node names.
     */
    int print_named_dendogram(
        cv::Mat & output,
        const DendogramNode & input
    );
} // namespace util

#endif // UTIL_CV_H
