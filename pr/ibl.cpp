#include "ibl.h"
#include "pr/p_norm.h"

void ibl1::train( const DataSet & dataset ) {
    nn = std::make_unique<NearestNeighbor>(
        std::make_unique<DataSet>( dataset.header() ),
        std::make_unique<EuclideanDistance>(0),
        1,
        false
    );

    auto it = dataset.begin();

    /* nn.edit_dataset() will be our conceptual descriptor. */
    nn->edit_dataset().push_back( DataEntry(*it) );
    ++hit;
    while( ++it != dataset.end() ) {
        auto category = nn->classify( *it );
        if( category == it->categories() )
            ++hit;
        else
            ++miss;

        nn->edit_dataset().push_back( DataEntry(*it) );
    }
}

int ibl1::hit_count() const {
    return hit;
}
int ibl1::miss_count() const {
    return miss;
}
const DataSet & ibl1::conceptual_descriptor() const {
    return nn->dataset();
}
const NearestNeighbor & ibl1::nearest_neighbor() const {
    return *nn;
}

void ibl2::train( const DataSet & dataset ) {
    nn = std::make_unique<NearestNeighbor>(
        std::make_unique<DataSet>( dataset.header() ),
        std::make_unique<EuclideanDistance>(0),
        1,
        false
    );

    auto it = dataset.begin();

    /* nn.edit_dataset() will be our conceptual descriptor. */
    nn->edit_dataset().push_back( DataEntry(*it) );
    ++hit;
    while( ++it != dataset.end() ) {
        auto category = nn->classify( *it );
        if( category == it->categories() )
            ++hit;
        else {
            ++miss;
            nn->edit_dataset().push_back( DataEntry(*it) );
        }
    }
}

int ibl2::hit_count() const {
    return hit;
}
int ibl2::miss_count() const {
    return miss;
}
const DataSet & ibl2::conceptual_descriptor() const {
    return nn->dataset();
}
const NearestNeighbor & ibl2::nearest_neighbor() const {
    return *nn;
}
