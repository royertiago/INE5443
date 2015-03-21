#ifndef DISTANCE_H
#define DISTANCE_H

#include <vector>

class DataSet;
class DataEntry;

struct DistanceCalculator {
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;
    virtual void normalize( const DataSet& ) = 0;
};


class NormalizingDistanceCalculator : public DistanceCalculator {
private:
    double tolerance;
    bool normalized;
    std::vector< double > minimum_value;
    std::vector< double > multiplicative_factor;
protected:
    double normalize( double value, std::size_t attribute_index ) const;
public:
    NormalizingDistanceCalculator( double tolerance );
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;
    virtual void normalize( const DataSet& ) override;
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
