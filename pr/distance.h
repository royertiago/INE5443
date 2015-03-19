#ifndef DISTANCE_H
#define DISTANCE_H

struct DistanceCalculator {
    virtual double compute( const double * begin1, const double * begin2 ) = 0;
};

class EuclideanDistance : public DistanceCalculator {
    unsigned size;
public:
    EuclideanDistance( unsigned size );
    virtual double compute( const double * begin1, const double * begin2 ) override;
};

struct ManhattanDistance : public DistanceCalculator {
public:
    unsigned size;
    ManhattanDistance( unsigned size );
    virtual double compute( const double * begin1, const double * begin2 ) override;
};

#endif // DISTANCE_H
