#include "pr/data_set.h"
#include <catch.hpp>

TEST_CASE( "Trivial DataSet member test", "[DataSet][trivial]" ) {
    DataSet dataset( std::vector<std::string>{ "X pos", "Y pos" },
                     std::vector<std::string>{ "Color" },
                     std::vector<DataEntry>{
                         DataEntry({1, 1},{"Blue"}),
                         DataEntry({2, 4},{"Red"}),
                         DataEntry({4, 0},{"Green"})
                     }
                    );

    REQUIRE( dataset.size() == 3 );
    REQUIRE( dataset.attribute_count() == 2 );
    REQUIRE( dataset.category_count() == 1 );

    CHECK( dataset.attribute_name(0) == "X pos" );
    CHECK( dataset.attribute_name(1) == "Y pos" );
    CHECK( dataset.category_name(0) == "Color" );

    auto it = dataset.begin();
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({1, 1},{"Blue"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({2, 4},{"Red"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({4, 0},{"Green"}) );

    ++it;
    CHECK( it == dataset.end() );
}
