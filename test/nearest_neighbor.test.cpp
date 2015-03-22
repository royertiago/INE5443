#include "pr/nearest_neighbor.h"
#include <catch.hpp>

#include "pr/data_set.h"
#include "pr/data_entry.h"
#include "pr/distance.h"

DataSet xy_data(
    std::vector<std::string>{"X pos", "Y pos"},
    std::vector<std::string>{"Type"},
    std::vector<DataEntry>{
        DataEntry({3, 4},{"A"}),
        DataEntry({6, 0},{"B"}),
        DataEntry({8, 0},{"A"}),
        DataEntry({6,-10},{"B"}),

        // Neutralize normalization
        DataEntry({-10,-10},{"C"}),
        DataEntry({10,10},{"C"})
    }
);

std::vector<std::string> category_A = {"A"};
std::vector<std::string> category_B = {"B"};
std::vector<std::string> category_C = {"C"};

TEST_CASE( "Euclidean Nearest Neighbor, NN=1, one category", "[nn][trivial]" ) {
    EuclideanDistance distance(0);
    NearestNeighbor nn(xy_data, distance, 1);

    CHECK( nn.classify(DataEntry({0,0},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,0},{})) == category_B );
    CHECK( nn.classify(DataEntry({0,4},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,7.0001},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,6.9999},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,10},{})) == category_A );
    CHECK( nn.classify(DataEntry({5,5},{})) == category_A );
    CHECK( nn.classify(DataEntry({8,8},{})) == category_C );
    CHECK( nn.classify(DataEntry({-5,-5},{})) == category_C );
}

TEST_CASE( "Manhattan Nearest Neighbor, NN=1, one category", "[nn][trivial]" ) {
    ManhattanDistance distance(0);
    NearestNeighbor nn(xy_data, distance, 1);

    CHECK( nn.classify(DataEntry({0,0},{})) == category_B );
    CHECK( nn.classify(DataEntry({3,0},{})) == category_B );
    CHECK( nn.classify(DataEntry({0,4},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,7.0001},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,6.9999},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,10},{})) == category_A );
    CHECK( nn.classify(DataEntry({5,5},{})) == category_A );
    CHECK( nn.classify(DataEntry({8,8},{})) == category_C );
    CHECK( nn.classify(DataEntry({-5,-5},{})) == category_C );
}

TEST_CASE( "Euclidean Nearest Neighbor, NN=2, one category", "[nn][two_neighbors]" ) {
    EuclideanDistance distance(0);
    NearestNeighbor nn(xy_data, distance, 2);

    CHECK( nn.classify(DataEntry({0,0},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,0},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,4},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,7.0001},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,6.9999},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,10},{})) == category_A );
    CHECK( nn.classify(DataEntry({5,5},{})) == category_A );
    CHECK( nn.classify(DataEntry({8,8},{})) == category_A );
    CHECK( nn.classify(DataEntry({-5,-5},{})) == category_B );
    CHECK( nn.classify(DataEntry({10,-5},{})) == category_B );
    CHECK( nn.classify(DataEntry({10,-10},{})) == category_B );
    CHECK( nn.classify(DataEntry({-10,10},{})) == category_C );
}

TEST_CASE( "Manhattan Nearest Neighbor, NN=2, one category", "[nn][two_neighbors]" ) {
    ManhattanDistance distance(0);
    NearestNeighbor nn(xy_data, distance, 2);

    CHECK( nn.classify(DataEntry({0,0},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,0},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,4},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,7.0001},{})) == category_A );
    CHECK( nn.classify(DataEntry({0,6.9999},{})) == category_A );
    CHECK( nn.classify(DataEntry({3,10},{})) == category_A );
    CHECK( nn.classify(DataEntry({5,5},{})) == category_A );
    CHECK( nn.classify(DataEntry({8,8},{})) == category_A );
    CHECK( nn.classify(DataEntry({-5,-5},{})) == category_B );
    CHECK( nn.classify(DataEntry({10,-5},{})) == category_B );
    CHECK( nn.classify(DataEntry({10,-10},{})) == category_B );
    CHECK( nn.classify(DataEntry({-10,10},{})) == category_C );
}
