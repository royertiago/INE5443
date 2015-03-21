#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <string>
#include <vector>

class DataSet;
struct DistanceCalculator;
class DataEntry;

class DistanceCalculator;

class NearestNeighbor {
    const DataSet & dataset;
    DistanceCalculator& distance;
    std::size_t neighbors; // Nearest Neighbor algorithm parameter

public:
    NearestNeighbor(
        const DataSet & dataset,
        DistanceCalculator& distance,
        std::size_t neighbors
    );

    std::vector< std::string > classify( const DataEntry & );
};

#endif // NEAREST_NEIGHBOR_H
