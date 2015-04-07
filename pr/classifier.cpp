#include <getopt.h>
#include <cstdlib> // std::exit
#include <memory>
#include "classifier.h"
#include "pr/data_set.h"
#include "pr/nearest_neighbor.h"
#include "pr/distance.h"

constexpr char Classifier::help_message[];

Classifier::Classifier( int argc, char ** argv ) {
    double tolerance = 0.1;
    bool euclidean = false;
    unsigned neighbors = 1;
    std::FILE * dataset_file = nullptr;

    static option options[] = {
        {"manhattan", no_argument, 0, 'm'},
        {"hamming", no_argument, 0, 'm'},
        {"euclidean", no_argument, 0, 'e'},
        {"neighbors", required_argument, 0, 'n'},
        {"normalize-tolerance", required_argument, 0, 't'},
        {"dataset", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
    };
    int opt;
    int dummy_option_index;
    while( (opt = getopt_long( argc, argv, "men:t:d:h",
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
            case 'd':
                if( (dataset_file = std::fopen(optarg, "r")) == nullptr ) {
                    std::fprintf( stderr, "Error opening file %s\n", optarg );
                    std::exit(1);
                }
                break;
            case 'h':
                std::printf( help_message, argv[0] );
                std::exit(0);
                break;
            default:
                std::fprintf( stderr, "Unknown parameter %c\n", optopt );
                std::exit(1);
        }
    }
    if( dataset_file == nullptr ) {
        std::fprintf( stderr, "No dataset chosen.\n" );
        std::exit(1);
    }

    // Command line options parsed, now we will initialize the variables.

    _dataset = std::make_unique<DataSet>(DataSet::parse( dataset_file ));

    std::fclose(dataset_file);

    if( euclidean )
        calculator = std::make_unique<EuclideanDistance>( tolerance );
    else
        calculator = std::make_unique<ManhattanDistance>( tolerance );

    nn = std::make_unique<NearestNeighbor>( *_dataset, *calculator, neighbors );
}

std::vector<std::string> Classifier::classify( const DataEntry & data ) {
    return nn->classify( data );
}

const DataSet & Classifier::dataset() const {
    return *_dataset;
}

/* The destructor must be here because the default destructor,
 * for std::unique_ptr, requires the full class specification.
 * This would defeat the purpose of the forward-declarations
 * in the beginning of the header file. */
Classifier::~Classifier() {}
