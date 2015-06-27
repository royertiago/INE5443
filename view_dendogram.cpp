/* Program that displays a dendogram based on a dataset.
 */
#include <cstdio>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "pr/data_set.h"
#include "pr/dendogram_node.h"
#include "pr/dendogram.h"
#include "util/cv.h"

int main() {
    DataSet dataset = DataSet::parse( stdin );
    auto dendogram = generate_dendogram(dataset, SimpleLinkage);

    cv::Mat img(600, 800, CV_8UC3, cv::Scalar(255, 255, 255) );
    util::print_dendogram( img, *dendogram );
    cv::imshow( "IBL", img );
    cv::waitKey();

    return 0;
}
