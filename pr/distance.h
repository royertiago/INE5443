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
    bool normalized;
    std::vector< double > minimum_value;
    std::vector< double > multiplicative_factor;
protected:
    double normalize( double value, std::size_t attribute_index );
public:
    NormalizingDistanceCalculator( double tolerance );
    virtual double operator()( const DataEntry&, const DataEntry& ) const = 0;
    virtual void normalize( const DataEntry& ) override;
};


struct EuclideanDistance : public NormalizingDistanceCalculator {
    EuclideanDistance( double normalizing_tolarance );
    virtual double compute( const DataEntry&, const DataEntry& ) override;
};


struct ManhattanDistance : public NormalizingDistanceCalculator {
    ManhattanDistance( double normalizing_tolerance );
    virtual double compute( const DataEntry&, const DataEntry& ) override;
};

#endif // DISTANCE_H
