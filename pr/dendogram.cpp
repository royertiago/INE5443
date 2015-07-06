#include <limits>
#include <list>
#include <sstream>
#include "pr/dendogram.h"
#include "pr/p_norm.h"

std::unique_ptr<DendogramNode> generate_dendogram(
    const DataSet & dataset,
    LinkageDistanceFunction distance
)
{
    std::list< std::unique_ptr<DendogramNode> > nodes;
    typedef std::list<std::unique_ptr<DendogramNode>>::iterator iterator;

    struct distance_data {
        iterator left;
        iterator right;
        double distance;

        bool has( iterator it ) {
            return left == it || right == it;
        }
    };

    std::list< distance_data > distances;
    typedef std::list< distance_data >::iterator distance_iterator;

    for( const DataEntry & entry : dataset )
        nodes.push_back( std::make_unique<DendogramNode>( &entry ) );

    for( iterator it = nodes.begin(); it != nodes.end(); ++it ) {
        iterator jt = it;
        for( ++jt; jt != nodes.end(); ++jt )
            distances.push_back( {it, jt, distance(**it, **jt)} );
    }

    while( nodes.size() != 1 ) {
        distance_data best;
        best.distance = std::numeric_limits<double>::max();

        // Find the closest pair
        for( const auto& dist : distances )
            if( dist.distance < best.distance )
                best = dist;

        // Merge the nodes
        nodes.push_back( std::make_unique<DendogramNode>(
            std::move(*best.left), std::move(*best.right), best.distance
        ));

        // Clean distance list and 'nodes'
        distance_iterator it = distances.begin();
        while( it != distances.end() )
            if( it->has( best.left ) || it->has( best.right ) )
                it = distances.erase( it );
            else
                ++it;

        nodes.erase( best.left );
        nodes.erase( best.right );

        /* Store new distances
         * We must take care to not measure the distance
         * between the last node and itself.
         */
        iterator jt = nodes.begin();
        iterator end = nodes.end();
        iterator target = --end;
        for( ; jt != end; ++jt )
            distances.push_back( {jt, target, distance(**jt, **target)} );
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

dendogram_classification_data classify_dendogram(
    const DendogramNode & root,
    int minClass,
    int maxClass,
    const DataSet & base
)
{
    if( maxClass > root.size() )
        throw "maxClass is greater than the number of leaves in the dendogram.";
    if( minClass > maxClass )
        throw "minClass > maxClass.";

    std::vector< const DendogramNode * > classes;
    classes.reserve( maxClass );
    classes.push_back( &root );

    /* We will iterate through every possible splitting of the dendogram
     * in k classes.
     * The list 'classes' will hold the list of nodes
     * that are at the top of its classes
     * when we divide the dendogram in $k$ different classes.
     *
     * Note that positional information about classes is irrelevant,
     * so we may use a std::vector instead of a std::list.
     */

    /* Splits the vector 'classes' in the node with highest linkage distance.
     * Returns the second-highest linkage distance in that vector.
     */
    auto split_classes = [&](){
        double highest_linkage_distance = 0.0;
        double second_highest_linkage_distance = 0.0;
        unsigned index_of_highest = 0;

        // First, find the correct index
        for( int i = 0; i < classes.size(); i++ ) {
            if( classes[i]->linkage_distance() > highest_linkage_distance ) {
                second_highest_linkage_distance = highest_linkage_distance;
                highest_linkage_distance = classes[i]->linkage_distance();
                index_of_highest = i;
            }
            if( classes[i]->linkage_distance() > second_highest_linkage_distance )
                second_highest_linkage_distance = classes[i]->linkage_distance();
        }

        // Now, split!
        const DendogramNode * old_class = classes[index_of_highest];
        classes[index_of_highest] = &old_class->left();
        classes.push_back( &old_class->right() );

        if( old_class->left().linkage_distance() > second_highest_linkage_distance )
            second_highest_linkage_distance = old_class->left().linkage_distance();
        if( old_class->right().linkage_distance() > second_highest_linkage_distance )
            second_highest_linkage_distance = old_class->right().linkage_distance();

        return second_highest_linkage_distance;
    };

    // Highest linkage distante when we last divided the 'classes' vector.
    double current_linkage_distance = root.linkage_distance();
    // Previous value of current_linkage_distance
    double previous_linkage_distance = root.linkage_distance();

    // First, we will add minClass classes to 'classes'.
    for( int i = 1; i < minClass; i++ ) {
        previous_linkage_distance = current_linkage_distance;
        current_linkage_distance = split_classes();
    }

    // dendogram_classification_data attributes.
    const double linkage_min_class = current_linkage_distance;
    double linkage_upper_limit = previous_linkage_distance;
    double linkage_lower_limit = current_linkage_distance;

    /* Now, we will iterate for every k in the range [minClass, maxClass]
     * and choose the one that maximizes the linkage delta.
     *
     * linkage delta is the difference between
     * the linkage distance of the class we just split
     * and the linkage distance of the class we will split next.
     */
    std::vector< const DendogramNode * > best_split = classes;
    double best_linkage_delta = previous_linkage_distance - current_linkage_distance;

    for( int k = minClass; k < maxClass; k++ ) {
        previous_linkage_distance = current_linkage_distance;
        current_linkage_distance = split_classes();
        if( previous_linkage_distance - current_linkage_distance > best_linkage_delta ) {
            best_linkage_delta = previous_linkage_distance - current_linkage_distance;
            best_split = classes;
            linkage_upper_limit = previous_linkage_distance;
            linkage_lower_limit = current_linkage_distance;
        }
    }

    // dendogram_classification_data attribute.
    const double linkage_max_class = current_linkage_distance;

    // Finally, construct and return the dataset.
    std::vector< std::string > attribute_names;
    for( int i = 0; i < base.attribute_count(); i++ )
        attribute_names.push_back( base.attribute_name(i) );
    std::vector< std::string > category_names{ "Class" };
    std::vector< DataEntry > entries;

    int class_number = 1;
    std::stringstream class_name;
    for( const DendogramNode * ptr : best_split ) {
        class_name.str( "" );
        class_name << "C" << class_number++;
        for( const auto& entry : *ptr )
            entries.emplace_back(
                std::vector<double>( entry.attributes() ),
                std::vector<std::string>{ class_name.str() },
                entry.name()
            );
    }

    return {
        DataSet(
            std::move( attribute_names ),
            std::move( category_names ),
            std::move( entries )
        ),
        (int) best_split.size(),
        linkage_min_class,
        linkage_upper_limit,
        linkage_lower_limit,
        linkage_max_class,
    };
}
