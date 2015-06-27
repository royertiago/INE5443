#include "pr/dendogram_node.h"
#include "pr/data_entry.h"
#include <catch.hpp>

TEST_CASE( "DendogramIterator", "[dendogram]" ) {
    DataEntry d1({},{},"1");
    DataEntry d2({},{},"2");
    DataEntry d3({},{},"3");
    DataEntry d4({},{},"4");
    DataEntry d5({},{},"5");

    // Terminals
    auto nt1 = std::make_unique<DendogramNode>( &d1 );
    auto nt2 = std::make_unique<DendogramNode>( &d2 );
    auto nt3 = std::make_unique<DendogramNode>( &d3 );
    auto nt4 = std::make_unique<DendogramNode>( &d4 );
    auto nt5 = std::make_unique<DendogramNode>( &d5 );

    // Second level
    auto ns1 = std::make_unique<DendogramNode>( std::move(nt1), std::move(nt2) );
    auto ns2 = std::make_unique<DendogramNode>( std::move(nt4), std::move(nt5) );

    // Third level
    auto nt = std::make_unique<DendogramNode>( std::move(ns1), std::move(nt3) );

    // Top-level
    auto top = std::make_unique<DendogramNode>( std::move(nt), std::move(ns2) );

    auto it = top->begin();
    CHECK( *it == d1 );
    REQUIRE( it != top->end() );
    ++it;
    CHECK( *it == d2 );
    REQUIRE( it != top->end() );
    ++it;
    CHECK( *it == d3 );
    REQUIRE( it != top->end() );
    ++it;
    CHECK( *it == d4 );
    REQUIRE( it != top->end() );
    ++it;
    CHECK( *it == d5 );
    REQUIRE( it != top->end() );
    ++it;
    CHECK( it == top->end() );
}
