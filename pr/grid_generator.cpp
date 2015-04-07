#include <algorithm>
#include <cfloat>
#include <cstdio>
#include <cstdlib>
#include "grid_generator.h"

constexpr unsigned GridGenerator::default_density;
using Iterator = GridGenerator::GridIterator;

void GridGenerator::expand( double d ) {
    _expand = d;
}
const double& GridGenerator::expand() const {
    return _expand;
}

void GridGenerator::density( const std::vector<unsigned>& v ) {
    _density = v;
}
const std::vector<unsigned>& GridGenerator::density() const {
    return _density;
}

void GridGenerator::calibrate( const DataSet & dataset ) {
    if( _density.size() != dataset.attribute_count() ) {
        std::fprintf( stderr, "Size of dimension vector"
                " do not match attribute count in dataset.\n" );
        std::exit(2);
    }

    std::size_t size = dataset.attribute_count();

    _density = std::vector<unsigned>( size, default_density );

    std::vector<double> min( size, DBL_MAX );
    std::vector<double> max( size, DBL_MIN );

    for( const DataEntry & entry : dataset ) {
        for( unsigned i = 0; i < size; i++ ) {
            min[i] = std::min(min[i], entry.attribute(i));
            max[i] = std::max(max[i], entry.attribute(i));
        }
    }

    step.resize( size );
    shift.resize( size );

    for( unsigned i = 0; i < size; i++ )
        step[i] = (max[i] - min[i]) * (1+2*_expand) / _density[i];

    for( unsigned i = 0; i < size; i++ )
        shift[i] = (max[i] - min[i]) * _expand - min[i];
}

DataEntry GridGenerator::operator()( const std::vector<unsigned> & index ) const {
    std::size_t size = _density.size();
    if( index.size() != size )
        throw "Index vector size incompatible with calibrated dataset.";

    std::vector< double > attributes( size );
    for( unsigned i = 0; i < size; i++ )
        attributes[i] = index[i] * step[i] - shift[i];

    return DataEntry( std::move(attributes), {} );
}

Iterator GridGenerator::begin() const {
    GridIterator it;
    it.index.resize( _density.size() );
    it.expired = false;
    it.grid = this;
    return it;
}

Iterator GridGenerator::end() const {
    GridIterator it;
    it.expired = true;
    it.grid = this;
    return it;
}


DataEntry Iterator::operator*() const {
    return grid->operator()( index );
}

Iterator & Iterator::operator++() {
    for( int i = index.size() - 1; ; i-- ) {
        if( i < 0 ) {
            expired = true;
            break;
        }
        index[i]++;
        if( index[i] > grid->density()[i] )
            index[i] = 0;
        else
            break;
    }
    return *this;
}

bool operator==( const Iterator & lhs, const Iterator & rhs ) {
    if( lhs.expired || rhs.expired )
        return rhs.expired && rhs.expired;

    return lhs.index == rhs.index;
}

bool operator!=( const Iterator & lhs, const Iterator & rhs ) {
    return !(rhs == lhs);
}
