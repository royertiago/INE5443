#include "pr/dendogram.h"
#include "pr/dendogram_node.h"
#include "pr/data_set.h"
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

TEST_CASE( "Dendogram builder", "[dendogram]" ) {
    DataSet dataset(
        std::vector<std::string>{"x", "y"},
        std::vector<std::string>{},
        std::vector<DataEntry>{
            DataEntry({0, 0}, {}, "A"),
            DataEntry({0, 1}, {}, "B"),
            DataEntry({2, 0}, {}, "C"),
            DataEntry({2, 1}, {}, "D"),
            DataEntry({1, 4}, {}, "E"),
        }
    );
    auto dendogram = generate_dendogram( dataset, SimpleLinkage );
    /* Structure:       dendogram
     *                  /       \
     *               left      right
     *              /         /     \
     *             /      child1    child2
     *            /        /  \      /  \
     *           E        A    B    C    D
     */

    REQUIRE( dendogram->structural() );
    const DendogramNode & left = dendogram->begin()->name() == "E" ?
        dendogram->left() : dendogram->right();
    const DendogramNode & right = dendogram->begin()->name() != "E" ?
        dendogram->left() : dendogram->right();

    SECTION( "Analysis of node `left`" ) {
        REQUIRE( left.leaf() );
        CHECK( left.data().name() == "E" );
    }
    SECTION( "Analysis of node `right`" ) {
        REQUIRE( right.structural() );
        bool child1_left = right.begin()->name() == "A" || right.begin()->name() == "B";
        const DendogramNode & child1 = child1_left ? right.left() : right.right();
        const DendogramNode & child2 = !child1_left ? right.left() : right.right();

        SECTION( "Analysis of node `child1`" ) {
            REQUIRE( child1.structural() );
            const DendogramNode & a = child1.begin()->name() == "A" ?
                child1.left() : child1.right();
            const DendogramNode & b = child1.begin()->name() != "A" ?
                child1.left() : child1.right();
            REQUIRE( a.leaf() );
            CHECK( a.data().name() == "A" );
            REQUIRE( b.leaf() );
            CHECK( b.data().name() == "B" );
        }

        SECTION( "Analysis of node `child2`" ) {
            REQUIRE( child2.structural() );
            const DendogramNode & c = child2.begin()->name() == "C" ?
                child2.left() : child2.right();
            const DendogramNode & d = child2.begin()->name() != "C" ?
                child2.left() : child2.right();
            REQUIRE( c.leaf() );
            CHECK( c.data().name() == "C" );
            REQUIRE( d.leaf() );
            CHECK( d.data().name() == "D" );
        }
    }
}
