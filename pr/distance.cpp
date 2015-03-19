/* Implementation of distance.h.
 */
#include <cmath>
#include "distance.h"

EuclideanDistance::EuclideanDistance( unsigned size ) :
    size(size)
{}

double EuclideanDistance::compute( const double * begin1, const double * begin2 ) {
    double sum = 0;
    for( int i = 0; i < size; i++ ) {
        sum = std::fma(*begin1 - *begin2, *begin1 - *begin2, sum);
        ++begin1;
        ++begin2;
    }
    return std::sqrt(sum);
};

ManhattanDistance::ManhattanDistance( unsigned size ) :
    size(size)
{}

double ManhattanDistance::compute( const double * begin1, const double * begin2 ) {
    double sum = 0;
    for( int i = 0; i < size; i++ ) {
        sum += std::fabs(*begin1 - *begin2);
        ++begin1;
        ++begin2;
    }
    return sum;
};
