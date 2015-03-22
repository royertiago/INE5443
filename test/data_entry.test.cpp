/* Unit test for pr/data_entry.h. */
#include "pr/data_entry.h"
#include <catch.hpp>

TEST_CASE( "Trivial DataEntry member test", "[DataEntry][trivial]" ) {

    SECTION( "Default constructor" ) {
        DataEntry data;
        REQUIRE( data.attribute_count() == 0 );
        REQUIRE( data.category_count() == 0 );
    }

    SECTION( "DataEntry with no categories" ) {
        DataEntry data( std::vector<double>{0.1, 2.5, -3.5, 4},
                        std::vector<std::string>()
                      );
        REQUIRE( data.attribute_count() == 4 );
        REQUIRE( data.category_count() == 0 );
        CHECK( data.attribute(0) == Approx(0.1) );
        CHECK( data.attribute(1) == 2.5 );
        CHECK( data.attribute(2) == -3.5 );
        CHECK( data.attribute(3) == 4.0 );
        /* Latter three numbers are exactly representable in floating point,
         * so the comparison should be exact. */
    }

    SECTION( "DataEntry with no attributes" ) {
        DataEntry data( std::vector<double>(),
                        std::vector<std::string>{"A", "B", "XYZ"}
                      );
        REQUIRE( data.attribute_count() == 0 );
        REQUIRE( data.category_count() == 3 );
        CHECK( data.category(0) == "A" );
        CHECK( data.category(1) == "B" );
        CHECK( data.category(2) == "XYZ" );
    }

    SECTION( "DataEntry with both categories and attributes" ) {
        DataEntry data( std::vector<double>{-1, 3.9},
                        std::vector<std::string>{"A", "D"}
                      );
        REQUIRE( data.attribute_count() == 2 );
        REQUIRE( data.category_count() == 2 );
        CHECK( data.attribute(0) == -1 );
        CHECK( data.attribute(1) == Approx(3.9) );
        CHECK( data.category(0) == "A" );
        CHECK( data.category(1) == "D" );
    }
}
