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
    mutable std::unique_ptr<DistanceCalculator> _distance;
    std::size_t neighbors; // Nearest Neighbor algorithm parameter
    mutable bool dirty;

public:
    NearestNeighbor(
        std::unique_ptr<DataSet> && dataset,
        std::unique_ptr<DistanceCalculator> && distance,
        std::size_t neighbors
    );
    NearestNeighbor() = default;

    /* The first function returns a const reference to the dataset.
     * The second returns a modifiable reference,
     * but the next data entry classification will trigger a (potentially costly)
     * recalibration of the distance calculator.
     */
    const DataSet & dataset() const;
    DataSet & edit_dataset();

    std::vector< std::string > classify( const DataEntry & ) const;
};

#endif // NEAREST_NEIGHBOR_H
