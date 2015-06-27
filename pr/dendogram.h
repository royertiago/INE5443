#ifndef DENDOGRAM_H
#define DENDOGRAM_H

// Utilities to build a dendogram from a DataSet.

#include <memory>
class DendogramNode;
class DataSet;

/* Type that represents a function that takes a pair of DendogramNodes
 * and return their relative distance.
 *
 * `LinkageDistanceFunction`s must be symmetric.
 */
using LinkageDistanceFunction =
    double( const DendogramNode &, const DendogramNode & );

/* Builds a dendogram.
 * The nodes will point to the values inside the dataset.
 */
std::unique_ptr<DendogramNode> generate_dendogram(
    const DataSet &,
    LinkageDistanceFunction
);

/* Built-in linkage distance calculators.
 */

/* Simple linkage: the distance is the smallest distance
 * between an entry in the first dataset and an entry in the second dataset.
 */
double SimpleLinkage( const DendogramNode &, const DendogramNode & );

// Full linkage: uses the greatest distance instead of the smallest.
double FullLinkage( const DendogramNode &, const DendogramNode & );

// Mean linkage: uses the average distance instead of the smallest.
double MeanLinkage( const DendogramNode &, const DendogramNode & );

#endif // DENDOGRAM_H
