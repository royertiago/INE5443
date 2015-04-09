#include "mahalanobis.h"
#include "pr/data_set.h"

void MahalanobisDistance::calibrate( const DataSet & dataset ) {
    cv::Mat covariance;
    cv::Mat mean; // discarded
    cv::Mat_<double> data( dataset.size(), 3 );

    unsigned i = 0;
    auto it = dataset.begin();
    for( ; i < dataset.size(); ++i, ++it ) {
        data( i, 0 ) = it->attribute(0);
        data( i, 1 ) = it->attribute(1);
        data( i, 2 ) = it->attribute(2);
    }

    cv::calcCovarMatrix( data, covariance, mean, cv::COVAR_NORMAL | cv::COVAR_ROWS );
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
