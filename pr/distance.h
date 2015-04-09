#ifndef DISTANCE_H
#define DISTANCE_H

#include <vector>

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
};


/* Utility class that normalizes the entries to the [0, 1] interval,
 * for each of the dimensions of the dataset.
 */
class NormalizingDistanceCalculator : public DistanceCalculator {
private:
    double tolerance;
    bool normalized;
    std::vector< double > minimum_value;
    std::vector< double > multiplicative_factor;

protected:
    /* Normalize the given value, interpreted as a DataEntry attribute.
     * The index used to differentiate between the different attributes.
     *
     * If the calculator was not calibrated, returns the value unmodified.
     *
     * This function is intended to be used by subclasses.
     */
    double normalize( double value, std::size_t attribute_index ) const;
public:

    /* Construct the "normalization engine" with the given tolerance.
     *
     * Suppose that the values of some dimension lies in the interval [min,max].
     * If tolerance is zero, then this interval will be mapped to [0, 1]
     * (by the method normalize()) through an affine function;
     * that is, the mapping is perfect.
     * But the input data can have values, for this dimension, below min or above max.
     * The tolerance handles exactly that: it "shrinks" the interval in the image
     * so that these values sill lie in the interval [0, 1]
     * under that affine mapping.
     *
     * For tolerance value t, the target interval will be [1/2 - x, 1/2 + x],
     * with x choosen so that the distance from 1/2 to 1/2 + x
     * plus t * that distance is equals to 1;
     * that is, t is the "padding ratio".
     * For instance, t = 0.5 gives the interval [0.25, 0.75].
     */
    NormalizingDistanceCalculator( double tolerance );

    /* This method must be implemented by subclasses. */
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;

    /* Normalizes the distance calculator.
     * This method must be invoked prior to invocation of normalize();
     * otherwise, normalize() will not modify the values.
     */
    virtual void calibrate( const DataSet& ) override;
};


struct EuclideanDistance : public NormalizingDistanceCalculator {
    EuclideanDistance( double normalizing_tolarance );
    virtual double operator()( const DataEntry&, const DataEntry& ) const override;
};


struct ManhattanDistance : public NormalizingDistanceCalculator {
    ManhattanDistance( double normalizing_tolerance );
    virtual double operator()( const DataEntry&, const DataEntry& ) const override;
};

#endif // DISTANCE_H
