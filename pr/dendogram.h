#ifndef DENDOGRAM_H
#define DENDOGRAM_H

// Utilities to build a dendogram from a DataSet.

#include <memory>
#include "pr/data_set.h"
#include "pr/dendogram_node.h"

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

/* Given two parameters minClass and maxClass,
 * this function will find the "optimum cut" k (minClass <= k <= maxClass)
 * and divide the dendogram in k different classes.
 * The returned DataSet is the same dataset referred by the DendogramNode,
 * but each DataEntry will have a single category.
 * (with name "C1", "C2", ..., or "Ck")
 *
 * (We will extract attribute names from the dataset 'base'.)
 *
 * By "optimum cut" we mean a $k$ that maximizes the difference
 * between the maximum distance linkage with $k$ and $k+1$ classes.
 * In terms of the dendogram visualization, we will cut the classes
 * in a place that maximizes the horizontal distance
 * between the two closest nodes.
 *
 * (Note that we will never put everything in a single class
 * unless the maxClass parameter forces us to.)
 */
DataSet classify_dendogram(
    const DendogramNode &,
    int minClass,
    int maxClass,
    const DataSet& base
);

#endif // DENDOGRAM_H
