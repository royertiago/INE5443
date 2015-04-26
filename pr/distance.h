#ifndef DISTANCE_H
#define DISTANCE_H

class DataSet;
class DataEntry;

struct DistanceCalculator {
    /* Compute the distance between the origin two DataEntries.
     */
    virtual double operator()( const DataEntry& o, const DataEntry& t ) const = 0;

    /* Calibrate the DistanceCalculator to the given dataset.
     *
     * For instance, this method could compute the extremum of the points
     * and normalize the distance;
     * or it can compute the covariance matrix for the Mahalanobis distance.
     */
    virtual void calibrate( const DataSet& ) = 0;

    virtual ~DistanceCalculator() = default;
};

#endif // DISTANCE_H
