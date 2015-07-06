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

/* Type of a function that is associated to some LinkageDistanceFunction d.
 * If the parameters are (n1, n2, d1, d2),
 * this function should return d(n1, n2);
 * but it may assume that n2 has two children c1 and c2,
 * such that d(n1, c1) == d1 and d(n1, c2) == d2.
 *
 * That is, this function should calculate the distance between two nodes,
 * given the distances between one node and the children of the other.
 */
using LinkageDistanceUpdateFunction =
    double( const DendogramNode &, const DendogramNode &, double, double );

/* Builds a dendogram.
 * The nodes will point to the values inside the dataset.
 */
std::unique_ptr<DendogramNode> generate_dendogram(
    const DataSet &,
    LinkageDistanceFunction,
    LinkageDistanceUpdateFunction
);

/* Simple linkage: the distance is the smallest distance
 * between an entry in the first dataset and an entry in the second dataset.
 */
LinkageDistanceFunction SimpleLinkage;
LinkageDistanceUpdateFunction SimpleLinkageUpdate;

// Full linkage: uses the greatest distance instead of the smallest.
LinkageDistanceFunction FullLinkage;
LinkageDistanceUpdateFunction FullLinkageUpdate;

// Mean linkage: uses the average distance instead of the smallest.
LinkageDistanceFunction MeanLinkage;
LinkageDistanceUpdateFunction MeanLinkageUpdate;

/* Given two parameters minClass and maxClass,
 * this function will find the "optimum cut" k (minClass <= k <= maxClass)
 * and divide the dendogram in k different classes.
 *
 * This function returns the following information:
 *  classified_dataset
 *      Same dataset referred by the DendogramNode,
 *      but each DataEntry will have a single category,
 *      whose names are "C1", "C2", ..., or "Ck".
 *      (Determining these classes is the core functionality of this function.)
 *
 *  classes
 *      Selected number of classes (the value $k$ above).
 *
 *  linkage_min_class
 *      Maximum linkage distance of top nodes with 'minClass' classes.
 *
 *  linkage_max_class
 *      Maximum linkage distance of top nodes with 'maxClass' classes.
 *
 *  linkage_lower_limit
 *      Maximum linkage distance of top nodes with $k$ classes.
 *
 *  linkage_upper_limit
 *      Maximum linkage distance of top nodes with $k-1$ classes.
 *
 *  Note that linkage_min_class <= linkage_upper_limit <=
 *      linkage_lower_limit <= linkage_max_class.
 *
 * (We will extract attribute names from the dataset 'base'.)
 *
 * By "optimum cut" we mean a $k$ that maximizes
 * linkage_upper_limit - linkage_lower_limit.
 * That is, the next class merge will be the "costlier"
 * of all merges that happen between minClass and maxClass.
 *
 * (Note that we will never put everything in a single class
 * unless the maxClass parameter forces us to.)
 */
struct dendogram_classification_data {
    DataSet classified_dataset;
    int classes;
    double linkage_min_class;
    double linkage_upper_limit;
    double linkage_lower_limit;
    double linkage_max_class;
};

dendogram_classification_data classify_dendogram(
    const DendogramNode &,
    int minClass,
    int maxClass,
    const DataSet& base
);

#endif // DENDOGRAM_H
