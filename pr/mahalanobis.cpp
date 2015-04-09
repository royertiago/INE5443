#include "mahalanobis.h"
#include "pr/data_set.h"

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
    cv::Mat_<double> mean(1, 3);
    mean(0, 0) = e1.attribute(0);
    mean(0, 1) = e1.attribute(1);
    mean(0, 2) = e1.attribute(2);

    cv::Mat_<double> entry(1, 3);
    entry(0, 0) = e2.attribute(0);
    entry(0, 1) = e2.attribute(1);
    entry(0, 2) = e2.attribute(2);

    return cv::Mahalanobis( entry, mean, inverse_covariance );
}
