#include "pr/distance.h"
#include <catch.hpp>
#include "pr/data_entry.h"

TEST_CASE( "Distance Calculator without normalization", "[distance]" ) {
    DataEntry x({0, 1, 5}, {});
    DataEntry y({1, -1, 3}, {});
    DataEntry z({0, 0, 0}, {});

    SECTION( "Euclidean Distance" ) {
        EuclideanDistance distance( 0 );
        CHECK( distance( x, y ) == Approx(3) );
        CHECK( distance( x, z ) == Approx(std::sqrt(26)) );
        CHECK( distance( y, z ) == Approx(std::sqrt(11)) );
    }

    SECTION( "Manhattan Distance" ) {
        ManhattanDistance distance( 0 );
        CHECK( distance( x, y ) == Approx(5) );
        CHECK( distance( x, z ) == Approx(6) );
        CHECK( distance( y, z ) == Approx(5) );
    }

    SECTION( "Euclidean Distance - different normalization" ) {
        EuclideanDistance distance( 1 );
        CHECK( distance( x, y ) == Approx(3) );
        CHECK( distance( x, z ) == Approx(std::sqrt(26)) );
        CHECK( distance( y, z ) == Approx(std::sqrt(11)) );
    }

    SECTION( "Manhattan Distance - different normalization" ) {
        ManhattanDistance distance( 1 );
        CHECK( distance( x, y ) == Approx(5) );
        CHECK( distance( x, z ) == Approx(6) );
        CHECK( distance( y, z ) == Approx(5) );
    }
}
