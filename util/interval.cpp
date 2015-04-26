/* Implementation of interval.h.
 */
#include <cmath>
#include "interval.h"


interval precision_interval( double p, int n, double z ) {
    double upper_left = p + z*z / (2*n);
    double root = z * std::sqrt( p * (1-p) / n + z*z /(4*n*n) );
    double lower = 1 + z*z / n;

    return interval{ (upper_left + root)/lower, (upper_left - root)/lower};
}

interval frequency_interval( double p, int n, double z ) {
    /* The formula is the same, its just the variable semantics that change.
     */
    return precision_interval(p, n, z);
}
