/* Implementation of p_norm.h.
 */
#include <cmath>
#include <tuple> // std::tie
#include "p_norm.h"
#include "pr/data_set.h"
#include "pr/data_entry.h"

NormalizingDistanceCalculator::NormalizingDistanceCalculator( double tolerance ) :
    tolerance( tolerance ),
    normalized( false )
{}

void NormalizingDistanceCalculator::calibrate( const DataSet & dataset ) {
    normalized = true;
    std::tie(multiplicative_factor, minimum_value)
        = dataset.normalizing_factor( tolerance );
}

double NormalizingDistanceCalculator::normalize( double value, std::size_t index ) const {
    if( !normalized ) return value;
    return multiplicative_factor[index] * (value - minimum_value[index]);
};


EuclideanDistance::EuclideanDistance( double normalizing_tolerance ) :
    NormalizingDistanceCalculator( normalizing_tolerance )
{}

double EuclideanDistance::operator()( const DataEntry& e1, const DataEntry& e2 ) const {
    double sum = 0;
    for( std::size_t i = 0; i < e1.attribute_count(); i++ ) {
        double v1 = this->normalize( e1.attribute(i), i );
        double v2 = this->normalize( e2.attribute(i), i );
        sum = std::fma(v1 - v2, v1 - v2, sum);
    }
    return std::sqrt(sum);
};

ManhattanDistance::ManhattanDistance( double normalizing_tolerance ) :
    NormalizingDistanceCalculator( normalizing_tolerance )
{}

double ManhattanDistance::operator()( const DataEntry& e1, const DataEntry& e2 ) const {
    double sum = 0;
    for( std::size_t i = 0; i < e1.attribute_count(); i++ ) {
        double v1 = this->normalize( e1.attribute(i), i );
        double v2 = this->normalize( e2.attribute(i), i );
        sum += std::fabs(v1 - v2);
    }
    return std::fabs(sum);
};
