const char help_text[] = 
"%s [options] --dataset=file\n"
"This program loads a dataset in the format specified by dadasets/format.md\n"
"and try to classify the incoming entries into one of the categories\n"
"defined in the dataset.\n"
"Each line in stdin is treated as a data entry and is classified\n"
"against the dataset. The guessed category is printed to stdout.\n"
"\n"
"Options:\n"
"\n"
" --manhattan\n"
" --hamming\n"
"     Chooses Hamming/mannhattan distance as proximity metric.\n"
"\n"
" --euclidean\n"
"     Chooses euclidean distance as proximity metric.\n"
"     This is the default.\n"
"\n"
" --neighbors=N\n"
"     Set the minimum number of neighbhors analysed\n"
"     with the Nearest Neighbhor algorithm.\n"
"     Default is 1.\n"
"\n"
" --normalize-tolerance=F\n"
"     Sets the normalization tolerance to F.\n"
"     (F should be a floating-point number greater than 0.)\n"
"     Default is 0.1.\n"
"\n"
"     When measuring distances, the datasets should be normalized\n"
"     to avoid having differences in scale influencing the measurements.\n"
"\n"
" --input=file\n"
"     Reads data from file instead of stdin.\n"
"\n"
" --output=file\n"
"     Writes data to file instead of stdout.\n"
"\n"
" --help\n"
"     Display this help and quit.\n"
"\n"
" --version\n"
"     Show program version and quit.\n"
;

#include <stdio.h>
#include "pr/nearest_neighbor.h"
#include "pr/data_entry.h"
#include "pr/distance.h"

int main() {
    // read args

    DataSet dataset = DataSet::parse( dataset_file );
    std::size_t attributes = dataset.attribute_count();

    std::unique_ptr<DistanceCalculator> calculator;
    if( manhattan )
        calculator = std::make_unique<Manhattan>( attributes, tolerance );
    else
        calculator = std::make_unique<Euclidean>( attributes, tolerance );

    NearestNeighbor nn( *distance, dataset );
    DataEntry entry;

    while( (entry = DataEntry.parse(infile, attributes)).attribute_count() == atributes ) {
        const char * delim = "";
        for( auto str : nn.classify(entry) ) {
            std::cout << delim << str;
            delim = ", ";
        }
        std::cout << '\n';
    }

    return 0;
}
