#include <algorithm>
#include <cfloat>
#include <chrono>
#include <list>
#include <map>
#include <random>
#include "ibl.h"
#include "pr/p_norm.h"
#include "util/interval.h"

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
    ++miss;
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
    ++miss;
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

ibl3::ibl3( double accepting_threshold, double rejecting_threshold ):
    accepting_threshold( accepting_threshold ),
    rejecting_threshold( rejecting_threshold ),
    _seed( std::chrono::system_clock::now().time_since_epoch().count() )
{}

double ibl3::do_distance( const DataEntry & lhs, const DataEntry & rhs ) const {
    return EuclideanDistance(0)( lhs, rhs );
}
void ibl3::do_update_weights( const DataEntry &, const DataEntry &, double ) {
    // no-op
}
void ibl3::seed( long long unsigned seed ) {
    _seed = seed;
}
long long unsigned ibl3::seed() const {
    return _seed;
}

void ibl3::train( const DataSet & dataset ) {
    std::mt19937 rng(_seed);

    if( dataset.category_count() >= 2 )
        throw "Current IBL3 implementation has only support for one category type.";

    auto it = dataset.begin();

    struct instance {
        DataEntry entry;

        /* How many times this entry was used to classify an entry.
         */
        int use_count;

        /* How many times the classification went right.
         */
        int correct_use_count;
    };

    /* The conceptual descriptor may grow and shrink during the execution
     * of the training, so we will only write to _conceptual_descriptor
     * an the end of the algorithm.
     *
     * Note we are using a std::list instead of a std::set.
     * Three reasons:
     * 1. 'instance' does not have operator<.
     * 2. std::list has a nice O(1) element removal through iterators.
     * 3. We do not need ordering anyway...
     */
    std::list< instance > conceptual_descriptor;

    /* Number of already processed instances in this training.
     */
    std::size_t trained_instances_count = 0;

    /* category_appearance_count[str] is how many times an entity of category str
     * already appeared in the training set.
     */
    std::map< std::string, std::size_t > category_appearance_count;

    /* Returns a std::pair with the the precision and frequency intervals
     * for that instance, respectively, given the acceptance threshold.
     */
    auto intervals = [&]( const instance & i, double z ) {
        double p; int n;
        p = (double) i.correct_use_count / i.use_count;
        n = i.use_count;
        interval precision = precision_interval( p, n, z );

        auto appearances = category_appearance_count[i.entry.category(0)];
        p = (double) appearances / trained_instances_count;
        n = trained_instances_count;
        interval frequency = frequency_interval( p, n, z );

        return std::make_pair( precision, frequency );
    };

    /* Returns true if the specified instance is acceptable;
     * that is, this instance is a good classifier.
     * Returns false otherwise.
     *
     * Project decision: if the instance was never used to classify something,
     * give it a chance and consider it acceptable.
     */
    auto acceptable = [&]( const instance & i ) {
        if( i.use_count == 0 ) return true;
        auto pair = intervals( i, this->accepting_threshold );
        return pair.second.max <= pair.first.min;
    };

    /* Returns true if the specified instance should be discarded;
     * that is, the classification properties of this instance
     * is below the desirable.
     * Returns false otherwise.
     *
     * Project decision: if the instance was never used to classify something,
     * then we refuse to discard it.
     */
    auto rejectable = [&]( const instance & i ) {
        if( i.use_count == 0 ) return false;
        auto pair = intervals( i, this->rejecting_threshold );
        return pair.first.max <= pair.second.min;
    };

    // The algoritm begins here.
    conceptual_descriptor.push_back( {*it, 0, 0} );
    miss++;
    category_appearance_count[it->category(0)]++;
    trained_instances_count++;

    while( ++it != dataset.end() ) {
        // First, lets find a closest acceptable instance.
        auto closest_acceptable = conceptual_descriptor.end();
        double distance_to_closest = DBL_MAX;
        for( auto jt = conceptual_descriptor.begin();
            jt != conceptual_descriptor.end();
            ++jt )
        {
            if( acceptable(*jt) )
                if( do_distance(jt->entry, *it) < distance_to_closest )
                    closest_acceptable = jt;
        }

        // If there is no acceptable instances in the conceptual descriptor,
        // let's choose a random entry.
        if( closest_acceptable == conceptual_descriptor.end() ) {
            int random = rng() % conceptual_descriptor.size();
            closest_acceptable = conceptual_descriptor.begin();
            std::advance( closest_acceptable, random );
            /* All the presentations of the algorithm I've seen
             * actually tell to choose 'r' as a random number between
             * 1 and the size of the conceptual descriptor,
             * and to define closest_acceptable as the r-closest instance to *it.
             * However, 'r' is never used again, so this is just a complicated way
             * of choosing a random element in the conceptual descriptor.
             */
        }

        /* Either there are acceptable instances in the conceptual descriptor
         * and closest_acceptable points to the closest of them;
         * or there is no acceptable instances and closest_acceptable
         * points to a random instance.
         * In the former case, we will put the acceptability
         * of the closest instance under judgement.
         * In the latter case, we may thought as if we were giving chance
         * to a random element of the set to "prove their worth"
         * as a possibly good classifier.
         */

        trained_instances_count++;
        category_appearance_count[it->category(0)]++;

        closest_acceptable->use_count++;
        if( closest_acceptable->entry.categories() == it->categories() ) {
            hit++;
            closest_acceptable->correct_use_count++;
        }
        else {
            miss++;
            conceptual_descriptor.push_back( {*it, 0, 0} );
        }

        /* Now, remove from the conceptual descriptor every bad classifier.
         * First, we need to save the current closest entry,
         * because the pointer closest_aceptable might get removed
         * form the conceptual descriptor, but we need that entry
         * to call do_update_weights.
         */
        DataEntry closest_entry = closest_acceptable->entry;
        double threshold = do_distance(closest_acceptable->entry, *it);
        auto jt = conceptual_descriptor.begin();
        while( jt != conceptual_descriptor.end() )
            if( do_distance(jt->entry, *it) <= threshold && rejectable(*jt) )
                jt = conceptual_descriptor.erase( jt );
            else
                ++jt;

        // And finnaly, update the metric.
        double lambda =
            std::max(
                category_appearance_count[it->category(0)],
                category_appearance_count[closest_entry.category(0)]
            )
            / (double) trained_instances_count;

        do_update_weights( *it, closest_acceptable->entry, lambda );

    } // while( it != dataset.end() )

    // Now, we must store the data in the conceptual_descriptor
    // inside our instance dataset _conceptual_descriptor.
    _conceptual_descriptor = dataset.header();

    for( instance & i : conceptual_descriptor )
        if( i.use_count > 0 )
            _conceptual_descriptor.push_back( std::move(i.entry) );
}

int ibl3::hit_count() const {
    return hit;
}
int ibl3::miss_count() const {
    return miss;
}
const DataSet & ibl3::conceptual_descriptor() const {
    return _conceptual_descriptor;
}
const NearestNeighbor & ibl3::nearest_neighbor() const {
    nn.reset( new NearestNeighbor(
        std::make_unique<DataSet>(_conceptual_descriptor),
        std::unique_ptr<DistanceCalculator>(new EuclideanDistance(0)),
        1,
        false
    ) );
    return *nn;
}


ibl4::ibl4( double accepting_threshold, double rejecting_threshold ):
    ibl3( accepting_threshold, rejecting_threshold )
{}
double ibl4::do_distance( const DataEntry & x, const DataEntry & y ) const {
    auto sqr = []( double d ) { return d * d; };
    double res = 0;
    for( std::size_t i = 0; i < weights.size(); i++ )
        res += sqr( weights[i] * (x.attribute(i) - y.attribute(i)) );

    return std::sqrt( res );
}
void ibl4::do_update_weights( const DataEntry & a, const DataEntry & b, double lambda ) {
    for( std::size_t i = 0; i < weights.size(); i++ ) {
        double d = std::fabs(a.attribute(i) - b.attribute(i));

        if( a.categories() == b.categories() )
            accumulated_weights[i] += (1 - lambda) * (1 - d);
        else
            accumulated_weights[i] += (1 - lambda) * d;

        normalized_weights[i] += (1 - lambda);

        weights[i] = std::max( accumulated_weights[i] / normalized_weights[i] - .5, 0. );
    }
}
void ibl4::train( const DataSet & dataset ) {
    weights.resize( dataset.attribute_count(), 1.0 );
    accumulated_weights.resize( dataset.attribute_count(), 1.0 );
    normalized_weights.resize( dataset.attribute_count(), 1.0 );
    ibl3::train( dataset );
}
