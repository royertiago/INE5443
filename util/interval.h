#ifndef UTIL_INTERVAL_H
#define UTIL_INTERVAL_H

/* Class that represents an interval.
 * Mainly used in the IBL3 algorithm.
 */
struct interval {
    double min;
    double max;
};

/* Returns the precision interval for some instance.
 * This interval tries to summarize how precise the instance was
 * during the run of the IBL3 algorithm.
 *
 * Parameters:
 *  p: instance precision; the ratio between
 *     the number of times this instance was chosen to classify something
 *     and the number of times this classification was correct.
 *  n: classification attempts; the number of times
 *     this instance was chosen to classify something.
 *     Note you need this number to compute p.
 *  z: Acceptance threshold. This parameter defines how strict the interval is;
 *     the larger z is, the larger the returned interval will be
 *     (thus, the stricter are the requirements over the current instance),
 *     tough the relation is not linear.
 */
interval precision_interval( double p, int n, double z );

/* Returns the frequency interval for some category.
 * (Note that, differently from precision_interval, this function
 * refers to some category, not to some specific instance.)
 * This interval tries to summarize how important an instance need to be
 * to be a good classifier for this category.
 *
 * Parameters:
 *  p: class frequency; the ratio between
 *     how many times the category appeared in the "current training set"
 *     and the size of the "current training set".
 *     (By "current training set" I mean the set of instances
 *     that were already processed.)
 *  n: Number of training instances; size of current conceptual descriptor.
 *  z: Acceptance threshold; the same as in precision_interval.
 */
interval frequency_interval( double p, int n, double z );

#endif // UTIL_INTERVAL_H
