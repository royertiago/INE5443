#include <algorithm>
#include <map>
#include "nearest_neighbor.h"
#include "pr/data_entry.h"
#include "pr/data_set.h"
#include "pr/distance.h"

NearestNeighbor::NearestNeighbor(
    std::unique_ptr<DataSet> && dataset,
    std::unique_ptr<DistanceCalculator> && distance,
    std::size_t neighbors
) :
    _dataset( std::move(dataset) ),
    _distance( std::move(distance) ),
    neighbors( neighbors ),
    dirty(false)
{
    _distance->calibrate(*_dataset);
}

const DataSet& NearestNeighbor::dataset() const {
    return *_dataset;
}

DataSet & NearestNeighbor::edit_dataset() {
    dirty = true;
    return *_dataset;
}

std::vector< std::string > NearestNeighbor::classify( const DataEntry & target ) const {
    if( dirty ) {
        _distance->calibrate(*_dataset);
        dirty = false;
    }

    std::vector< std::pair<double, const DataEntry *> > nearest;
    for( const DataEntry & entry : *_dataset )
        nearest.emplace_back( (*_distance)( entry, target ), &entry );

    std::sort( nearest.begin(), nearest.end() );

    std::vector< std::map<std::string, unsigned> > votes( _dataset->category_count() );
    /* votes[i] represents the votes of the nearer neighbors
     * for the ith category that the target entry will be classified.
     * Thus, for instance, votes[0]["Iris-versicolor"] == 4
     * means that there are four votes for the first category
     * to be "Iris-setosa".
     *
     * We begin by processing the first `this->neighbors`
     * mandatory votes. */

    auto it = nearest.begin();
    for( unsigned i = 0; i < neighbors; ++i, ++it ) {
        if( it == nearest.end() )
            throw "Too few entries in dataset to categorize target entry.";
        for( unsigned j = 0; j < _dataset->category_count(); j++ )
            votes[j][it->second->category(j)]++;
    }

    std::vector< std::string > categories( _dataset->category_count() );
    std::vector< unsigned > max_votes( _dataset->category_count() );
    /* categories[i] is the category with the highest number of votes
     * for the ith category type,
     * or "" in the event of a draw.
     *
     * First, we will process the current votes.
     * If there are draws, we will get more votes
     * from the nearest neighbors after the first `this->neighbors`. */

    for( unsigned i = 0; i < _dataset->category_count(); ++i ) {
        for( auto pair: votes[i] ) {
            if( pair.second > max_votes[i] ) {
                categories[i] = pair.first;
                max_votes[i] = pair.second;
            }
            else if( pair.second == max_votes[i] ) // there is a draw
                categories[i] = "";
        }
    }

    /* Now, the tricky part: resolving draws.
     *
     * We will do this by considering the next-nearest neighbor;
     * if the draw persists
     * (for instance, there can be two votes for A,
     * one vote for B and two for C, and the next vote is for B),
     * we analyse yet another neighbor, and so on.
     * To guarantee termination,
     * once a tie is broken,
     * we will not consider votes for that category.
     * That is, categories are independent.
     *
     * Claim:
     * If there are C different categories for a single index
     * and we are considering N neighbors,
     * in the worst-case scenario we the need to investigate
     *  (C-2) * floor(N/2) + 1
     * neighbors in total to solve a tie with the first N entries.
     *
     * Proof.
     * If we reach a draw with V votes and T tied categories,
     * whenever the next investigated entry falls in any of these T categories,
     * the tie is broken.
     * The other C - T categories must have at most V votes
     * (since V is the maximum number of votes),
     * so we will find at most V entries in each of these categories.
     * before breaking the tie.
     * Thus, after analyzing at most
     *  (C - T) * V + 1
     * entries, we resolve the draw.
     * Now, T is at least two (otherwise its not a tie).
     * And, since V is determined in the first N entries,
     * 2*V is at most N, which gives the maximum value of
     * floor(N/2) for V.
     * Combining both estimates gives the maximum value
     *  (C-2) * floor(N/2) + 1.
     * Now observe that, if N is even, this maximum is reached.
     * If in the first N neighbors we have exactly N/2 votes
     * for two different categories,
     * and all votes are "wasted" in these two categories,
     * each of the C - 2 remaining categories have no votes.
     * So, we can have (C-2)*V more entries that do not untie
     * this category. As V = N/2 = floor(N/2),
     * the maximum is reached.
     * For odd N, one vote goes to any of the C-2 other categories,
     * so the maximum is (C-2)*floor(N/2).
     */
    bool draws = true;
    while( draws ) {
        draws = false;
        for( unsigned i = 0; i < _dataset->category_count(); ++i ) {
            if( categories[i] == "" ) {
                // We need a new vote, but we might not have more voters.
                if( it == nearest.end() )
                    throw "Too few entries in dataset to resolve the tie.";
                std::string next_vote = it->second->category(i);

                votes[i][next_vote]++;
                if( votes[i][next_vote] > max_votes[i] )
                    categories[i] = next_vote;
                else
                    draws = true;
            }
        }
        ++it;
    }

    return categories;
}

/* The destructor must be here because std::unique_ptr's default destructor
 * requires the full class specification.
 * This would defeat the purpose of the forward-declarations
 * in the beginning of the header file. */
NearestNeighbor::~NearestNeighbor() {}
