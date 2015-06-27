#include <limits>
#include <list>
#include "pr/dendogram.h"
#include "pr/dendogram_node.h"
#include "pr/data_set.h"
#include "pr/p_norm.h"

std::unique_ptr<DendogramNode> generate_dendogram(
    const DataSet & dataset,
    LinkageDistanceFunction distance
)
{
    std::list< std::unique_ptr<DendogramNode> > nodes;
    typedef std::list<std::unique_ptr<DendogramNode>>::iterator iterator;
    /* Simple brute-force algorithm.
     * Start with a list of leaf dendogram nodes,
     * and merge them as the algorithm proceeds.
     */

    for( const DataEntry & entry : dataset )
        nodes.push_back( std::make_unique<DendogramNode>( &entry ) );

    while( nodes.size() != 1 ) {
        iterator best_left;
        iterator best_right;
        double best = std::numeric_limits<double>::max();

        // Find the closest pair
        for( iterator it = nodes.begin(); it != nodes.end(); ++it ) {
            iterator jt = it;
            for( ++jt; jt != nodes.end(); ++jt ) {
                double d = distance( **it, **jt );
                if( d < best ) {
                    best = d;
                    best_left = jt;
                    best_right = it;
                }
            }
        }

        // Merge
        nodes.push_back( std::make_unique<DendogramNode>(
            std::move(*best_left), std::move(*best_right)
        ));
        nodes.erase( best_left );
        nodes.erase( best_right );
    }
    return std::move( nodes.front() );
}

double SimpleLinkage( const DendogramNode & a, const DendogramNode & b ) {
    EuclideanDistance dist(0.0);
    double d = std::numeric_limits<double>::max();
    for( auto entry1 : a )
        for( auto entry2 : b )
            d = std::min( d, dist(entry1, entry2) );
    return d;
}

double FullLinkage( const DendogramNode & a, const DendogramNode & b ) {
    EuclideanDistance dist(0.0);
    double d = -std::numeric_limits<double>::max();
    for( auto entry1 : a )
        for( auto entry2 : b )
            d = std::max( d, dist(entry1, entry2) );
    return d;
}

double MeanLinkage( const DendogramNode & a, const DendogramNode & b ) {
    EuclideanDistance dist(0.0);
    double d = 0;
    int count = 0;
    for( auto entry1 : a )
        for( auto entry2 : b ) {
            count++;
            d += dist(entry1, entry2);
        }
    return d / count;
}
