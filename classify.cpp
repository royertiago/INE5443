namespace command_line {
    const char help_message[] =
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
"\n"
"--euclidean\n"
"    Chooses euclidean distance as proximity metric.\n"
"    This is the default.\n"
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
"--input <file>\n"
"    Reads data from file instead of stdin.\n"
"\n"
"--output <file>\n"
"    Writes data to file instead of stdout.\n"
"\n"
"--help\n"
"    Display this help and quit.\n"
"\n"
"--version\n"
"    Show program version and quit.\n"
;
} // namespace command_line

#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include "pr/data_entry.h"
#include "pr/data_set.h"
#include "pr/distance.h"
#include "pr/nearest_neighbor.h"

namespace command_line {
    double tolerance = 0.1;
    bool euclidean = false;
    unsigned neighbors = 1;
    std::FILE * dataset;

    /* We will use freopen to reset stdin and stdout
     * in the presence of --input or --output options. */
    void parse( int argc, char ** argv ) {
        static option options[] = {
            {"manhattan", no_argument, 0, 'm'},
            {"hamming", no_argument, 0, 'm'},
            {"euclidean", no_argument, 0, 'e'},
            {"neighbors", required_argument, 0, 'n'},
            {"normalize-tolerance", required_argument, 0, 't'},
            {"input", required_argument, 0, 'i'},
            {"output", required_argument, 0, 'o'},
            {"dataset", required_argument, 0, 'd'},
            {"help", no_argument, 0, 'h'},
            {"version", no_argument, 0, 'v'},
            {0, 0, 0, 0},
        };
        int opt;
        int dummy_option_index;
        while( (opt = getopt_long( argc, argv, "men:t:i:o:d:hv",
                    options, &dummy_option_index
                )) != -1 ) {
            switch( opt ) {
                case 'm':
                    euclidean = false;
                    break;
                case 'e':
                    euclidean = true;
                    break;
                case 'n':
                    if( std::sscanf( optarg, "%u", &neighbors ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( neighbors == 0 ) {
                        std::fprintf( stderr, "Too few neighbors\n" );
                        std::exit(1);
                    }
                    break;
                case 't':
                    if( std::sscanf( optarg, "%lf", &tolerance ) != 1 ) {
                        std::fprintf( stderr, "Not a valid number: %s\n", optarg );
                        std::exit(1);
                    }
                    if( tolerance < 0 ) {
                        std::fprintf( stderr, "Invalid negative tolerance\n" );
                        std::exit(1);
                    }
                    break;
                case 'i':
                    if( std::freopen(optarg, "r", stdin) == nullptr ) {
                        std::fprintf( stderr, "Error opening file %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'o':
                    if( std::freopen(optarg, "w", stdout) == nullptr ) {
                        std::fprintf( stderr, "Error opening file %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'd':
                    if( (dataset = std::fopen(optarg, "r")) == nullptr ) {
                        std::fprintf( stderr, "Error opening file %s\n", optarg );
                        std::exit(1);
                    }
                    break;
                case 'h':
                    /* We print to stderr to allow for printing
                     * even after stdout was reopened above. */
                    std::fprintf( stderr, help_message, argv[0] );
                    std::exit(0);
                    break;
                case 'v':
                    std::fprintf( stderr, "0.1\n" );
                    std::exit(0);
                    break;
                default:
                    std::fprintf( stderr, "Unknown parameter %c\n", optopt );
                    std::exit(1);
            }
        }
        if( dataset == nullptr ) {
            std::fprintf( stderr, "No dataset chosen.\n" );
            std::exit(1);
        }
    } // void parse(int, char**)
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( argc, argv );

    DataSet dataset = DataSet::parse( command_line::dataset );

    std::unique_ptr<DistanceCalculator> calculator;
    if( command_line::euclidean )
        calculator = std::make_unique<EuclideanDistance>( command_line::tolerance );
    else
        calculator = std::make_unique<ManhattanDistance>( command_line::tolerance );

    NearestNeighbor nn( dataset, *calculator, command_line::neighbors );

    DataEntry entry;
    std::size_t attribute_count = dataset.attribute_count();
    while( true ) {
        entry = DataEntry::parse(stdin, attribute_count );
        if( entry.attribute_count() != attribute_count )
            break;

        DataEntry({},nn.classify(entry)).write( stdout );
    }
    return 0;
}
