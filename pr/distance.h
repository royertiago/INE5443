#ifndef DISTANCE_H
#define DISTANCE_H

class DataSet;
class DataEntry;

struct DistanceCalculator {
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;
    virtual void normalize( const DataEntry& ) = 0;
};


class NormalizingDistanceCalculator : public DistanceCalculator {
private:
    double tolerance;
protected:
    std::vector< double > minimum_value;
    std::vector< double > multiplicative_factor;
public:
    NormalizingDistanceCalculator( double tolerance );
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;
    virtual void normalize( const DataEntry& ) override;
};


class EuclideanDistance : public NormalizingDistanceCalculator {
    unsigned size;
public:
    EuclideanDistance( unsigned size, double normalizing_tolarance );
    virtual double compute( const double * begin1, const double * begin2 ) override;
};


struct ManhattanDistance : public NormalizingDistanceCalculator {
public:
    unsigned size;
    ManhattanDistance( unsigned size, double normalizing_tolerance );
    virtual double compute( const double * begin1, const double * begin2 ) override;
};

#endif // DISTANCE_H
