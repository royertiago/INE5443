#ifndef NEAREST_NEIGHBOR_H
#define NEAREST_NEIGHBOR_H

#include <string>
#include <vector>
#include "pr/data_entry.h"
#include "pr/data_set.h"

class NearestNeighbor {
    DistanceCalculator& distance;
    std::size_t _n; // Nearest Neighbor algorithm parameter
    const DataSet & data_set;

public:
    NearestNeighbor( const DataSet &, std::unique_ptr<DistanceCalculator>&& );
    std::size_t n() const;
    void n( std::size_t );

    std::vector< std::string > classify( const DataEntry & );
};

#endif // NEAREST_NEIGHBOR_H
