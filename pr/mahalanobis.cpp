#include <cmath>
#include "mahalanobis.h"
#include "pr/data_set.h"
#include "util/cv.h"

void MahalanobisDistance::calibrate( const DataSet & dataset ) {
    std::size_t dim = dataset.attribute_count();
    std::vector<double> mean(dim);
    for( const auto & entry : dataset )
        for( unsigned i = 0; i < dim; i++ )
            mean[i] += entry.attribute(i);

    for( unsigned i = 0; i < dim; i++ )
        mean[i] /= dataset.size() - 1;

    // mean[i] is the average of the attribute(i).
    cv::Mat_<double> covariance(dim, dim);
    for( const auto & entry : dataset )
        for( unsigned i = 0; i < dim; i++ )
            for( unsigned j = 0; j < dim; j++ )
                covariance(i, j) +=
                    (entry.attribute(i) - mean[i]) *
                    (entry.attribute(j) - mean[j]);

    cv::invert( covariance, inverse_covariance );
}

double MahalanobisDistance::operator()(
    const DataEntry & e1,
    const DataEntry & e2
) const {
    cv::Mat_<double> x = util::matFromEntry(e1);
    cv::Mat_<double> y = util::matFromEntry(e2);
    cv::Mat_<double> vec = x - y;

    cv::Mat_<double> val = vec * inverse_covariance * vec.t();

    return std::sqrt( val(0,0) );
}
