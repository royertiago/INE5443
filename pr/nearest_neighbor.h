#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <string>
#include <memory>
#include <vector>

class DataSet;
struct DistanceCalculator;
class DataEntry;

class NearestNeighbor {
    std::unique_ptr<DataSet> _dataset;
    std::unique_ptr<DistanceCalculator> _distance;
    std::size_t neighbors; // Nearest Neighbor algorithm parameter

public:
    NearestNeighbor(
        std::unique_ptr<DataSet> && dataset,
        std::unique_ptr<DistanceCalculator> && distance,
        std::size_t neighbors
    );
    NearestNeighbor() = default;

    std::vector< std::string > classify( const DataEntry & ) const;
};

#endif // NEAREST_NEIGHBOR_H
