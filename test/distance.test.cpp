#include <catch.hpp>
#include "pr/distance.h"

TEST_CASE( "Distance Calculator", "[distance_calculator]" ) {
    double x[] = {0, 1, 5};
    double y[] = {1, -1, 3};
    double z[] = {0, 0, 0};
    DistanceCalculator * c;
    
    SECTION( "Euclidean Distance" ) {
        c = new EuclideanDistance( 3 );
        CHECK( c->compute( x, y ) == Approx(3) );
        CHECK( c->compute( x, z ) == Approx(std::sqrt(26)) );
        CHECK( c->compute( y, z ) == Approx(std::sqrt(11)) );
        delete c;
    }

    SECTION( "Manhattan Distance" ) {
        c = new ManhattanDistance( 3 );
        CHECK( c->compute( x, y ) == Approx(5) );
        CHECK( c->compute( x, z ) == Approx(6) );
        CHECK( c->compute( y, z ) == Approx(5) );
        delete c;
    }
}
