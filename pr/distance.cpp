/* Implementation of distance.h.
 */
#include <cmath>
#include "distance.h"
#include "pr/data_set.h"
#include "pr/data_entry.h"

NormalizingDistanceCalculator::NormalizingDistanceCalculador( double tolerance ) :
    tolerance( tolerance ),
    normalized( false ),
{}

void NormalizingDistanceCalculator::normalize( const DataSet & dataset ) {
    normalized = true;
    minimum_value = std::vector<double>( dataset.attribute_count(), DBL_MAX );
    auto minimum_value = std::vector<double>( dataset.attribute_count(), DBL_MIN );
    for( const DataEntry & entry : dataset )
        for( std::size_t i = 0; i < dataset.attribute_count(); i++ ) {
            minimum_value[i] = std::min(minimum_value[i], entry.attribute(i));
            maximum_value[i] = std::max(maximum_value[i], entry.attribute(i));
        }

    multiplicative_factor.resize( dataset.attribute_count() );
    for( std::size_t i = 0; i < dataset.attribute_count(); i++ ) {
        double distance = maximum_value[i] - minimum_value[i];
        double tolerance_radius = tolerance * distance;
        maximum_value[i] -= tolerance_radius;
        minimum_value[i] += tolerance_radius;
        multiplicative_factor[i] = 1/(maximum_value[i] - minimum_value[i]);
    }
}

void NormalizingDistanceCalculator::normalize( double value, std::size_t index ) {
    if( !normalized ) return value;
    return multiplicative_factor[i] * (value - minimum_value[i]);
};


EuclideanDistance::EuclideanDistance( double normalizing_tolerance ) :
    NormalizingDistanceCalculator( normalizing_tolerance )
{}

double EuclideanDistance::compute( const DataEntry& e1, const DataEntry& e2 ) const {
    double sum = 0;
    for( std::size_t i = 0; i < e1.attribute_count(); i++ ) {
        double v1 = this->normalize( e1.attribute(i) );
        double v2 = this->normalize( e2.attribute(i) );
        sum = std::fma(v1 - v2, v1 - v2, sum);
    }
    return std::sqrt(sum);
};

ManhattanDistance::ManhattanDistance( double normalizing_tolerance ) :
    NormalizingDistanceCalculator( normalizing_tolerance )
{}

double ManhattanDistance::compute( const double * begin1, const double * begin2 ) {
    double sum = 0;
    for( std::size_t i = 0; i < e1.attribute_count(); i++ ) {
        double v1 = this->normalize( e1.attribute(i) );
        double v2 = this->normalize( e2.attribute(i) );
        sum += std::fabs(v1 - v2);
    }
    return std::sqrt(sum);
};
