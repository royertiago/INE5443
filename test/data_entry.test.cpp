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

TEST_CASE( "DataEntry equality/inequality operators", "[DataEntry][operators]" ) {
    DataEntry e1( std::vector<double>{-1, 1},
                  std::vector<std::string>{"A"}
                );
    DataEntry e2( std::vector<double>{-1, 1},
                  std::vector<std::string>{"A"}
                );
    DataEntry e3( std::vector<double>{-1, 1, 1},
                  std::vector<std::string>{"A"}
                );
    DataEntry e4( std::vector<double>{-1, 1},
                  std::vector<std::string>{"B"}
                );
    DataEntry e5( std::vector<double>{-1, 1.00000001},
                  std::vector<std::string>{"B"}
                );
    DataEntry e6( std::vector<double>{-1, 1.00000001},
                  std::vector<std::string>{"B"}
                );

    SECTION( "Tests against e1" ) {
        CHECK( e1 == e2 );
        CHECK_FALSE( e1 != e2 );
        CHECK_FALSE( e1 == e3 );
        CHECK( e1 != e3 );
        CHECK_FALSE( e1 == e4 );
        CHECK( e1 != e4 );
        CHECK_FALSE( e1 == e5 );
        CHECK( e1 != e5 );
        CHECK_FALSE( e1 == e6 );
        CHECK( e1 != e6 );
    }

    SECTION( "Tests against e2 (for transitivity)" ) {
        CHECK_FALSE( e2 == e3 );
        CHECK( e2 != e3 );
        CHECK_FALSE( e2 == e4 );
        CHECK( e2 != e4 );
        CHECK_FALSE( e2 == e5 );
        CHECK( e2 != e5 );
        CHECK_FALSE( e2 == e6 );
        CHECK( e2 != e6 );
    }

    SECTION( "Floating point checks" ) {
        CHECK_FALSE( e4 == e5 );
        CHECK( e4 != e5 );
        CHECK_FALSE( e4 == e6 );
        CHECK( e4 != e6 );
        CHECK( e5 == e6 );
        CHECK_FALSE( e5 != e6 );
    }
}

TEST_CASE( "DataEntry std::initializer_list constructor",
           "[DataEntry][initializer_list]" )
{
    SECTION( "Default constructor" ) {
        DataEntry data;
        DataEntry il( {}, {} );
        CHECK( data == il );
    }

    SECTION( "DataEntry with no categories" ) {
        DataEntry data( std::vector<double>{0.1, 2.5, -3.5, 4},
                        std::vector<std::string>()
                      );
        DataEntry il( {0.1, 2.5, -3.5, 4}, {} );
        CHECK( data == il );
    }

    SECTION( "DataEntry with no attributes" ) {
        DataEntry data( std::vector<double>(),
                        std::vector<std::string>{"A", "B", "XYZ"}
                      );
        DataEntry il( {}, {"A", "B", "XYZ"} );
        CHECK( data == il );
    }

    SECTION( "DataEntry with both categories and attributes" ) {
        DataEntry data( std::vector<double>{-1, 3.9},
                        std::vector<std::string>{"A", "D"}
                      );
        DataEntry il( {-1, 3.9}, {"A", "D"} );
        CHECK( data == il );
    }
}
