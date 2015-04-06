#ifndef CLASSIFIER_H
#define CLASSIFIER_H

/* Class that encapsulates the classification process.
 * The constructor is based on argc and argv;
 * see the classifier_help_message for details.
 */

#include <memory>
#include <string>
#include <vector>

#include "pr/data_entry.h"

class DataSet;
struct DistanceCalculator;
class NearestNeighbor;

class Classifier {
    std::unique_ptr<DataSet> dataset;
    std::unique_ptr<DistanceCalculator> calculator;
    std::unique_ptr<NearestNeighbor> nn;

    static constexpr char help_message[] =
"%s [options] --dataset <file>\n"
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
"    This is the only obrigatory option;\n"
"    it is listed as an \"option\" because the agrument order do not matter.\n"
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

public:
    Classifier( int argc, char ** argv );
    ~Classifier();

    std::size_t dataset_attribute_count() const;

    std::vector< std::string > classify( const DataEntry & );
};

#endif // CLASSIFIER_H
