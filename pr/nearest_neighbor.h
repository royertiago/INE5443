#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <string>
#include <vector>

class DistanceCalculator;

class NearestNeighbor {
    const DataSet & dataset;
    DistanceCalculator& distance;
    std::size_t _n; // Nearest Neighbor algorithm parameter

public:
    NearestNeighbor(
        const DataSet & dataset,
        DistanceCalculator& distance,
        std::size_t neighbors
    );

    std::vector< std::string > classify( const DataEntry & );
};

#endif // NEAREST_NEIGHBOR_H
