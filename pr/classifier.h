#ifndef CLASSIFIER_H
#define CLASSIFIER_H

/* Helper function that constructs a NearestNeighbor object
 * from command line options.
 */

#include "cmdline/args.h"
#include "pr/nearest_neighbor.h"

constexpr char classifier_help_message[] =
" [options] --dataset <file>\n"
"This program loads a dataset in the format specified by dadasets/format.md\n"
"and try to classify the incoming entries into one of the categories\n"
"defined in the dataset.\n"
"Each line in stdin is treated as a data entry and is classified\n"
"against the dataset. The guessed category is printed to stdout.\n"
"\n"
"Options:\n"
"\n"
"--dataset <file>\n"
"    Selects file as the data set against which\n"
"    the entries are to be classified.\n"
"    If unspecified, reads from stdin.\n"
"    (Note that reading from stdin might conflict with the entry input.)\n"
"\n"
"--manhattan\n"
"--hamming\n"
"    Chooses Hamming/mannhattan distance as proximity metric.\n"
"    This is the default.\n"
"\n"
"--euclidean\n"
"    Chooses euclidean distance as proximity metric.\n"
"\n"
"--neighbors <N>\n"
"    Set the minimum number of neighbhors analysed\n"
"    with the Nearest Neighbhor algorithm.\n"
"    Default is 1.\n"
"\n"
"--normalize\n"
"--no-normalize\n"
"    Enable or disable the normalization done by the distance calculator.\n"
"    The normalization attempts to reduce the weight of attributes\n"
"    that are too spaced in comparison to more condensed attributes.\n"
"    Default: normalize.\n"
"\n"
"--normalize-tolerance <F>\n"
"    Sets the normalization tolerance to F.\n"
"    (F should be a floating-point number greater than 0.)\n"
"    Default is 0.1.\n"
"\n"
"    When measuring distances, the datasets should be normalized\n"
"    to avoid having differences in scale influencing the measurements.\n"
"\n"
"--help\n"
"    Display this help and quit.\n"
;

std::unique_ptr<NearestNeighbor> generate_classifier( cmdline::args&& );

#endif // CLASSIFIER_H
