#ifndef PR_MAHALANOBIS_H
#define PR_MAHALANOBIS_H

#include <opencv2/core/core.hpp>
#include "pr/distance.h"

/* DistanceCalculator that calculates the distances
 * using the Mahalanobis distance.
 */
class MahalanobisDistance : public DistanceCalculator {
    cv::Mat inverse_covariance;
public:

    /* Computes the Mahalanobis distance, assuming that m is the mean.
     */
    virtual double operator()( const DataEntry& m, const DataEntry& x ) const override;
    virtual void calibrate( const DataSet& ) override;
};

#endif // PR_MAHALANOBIS_H
