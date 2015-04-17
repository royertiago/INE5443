#include <getopt.h>
#include <cstdlib> // std::exit
#include <memory>
#include "classifier.h"
#include "pr/data_set.h"
#include "pr/nearest_neighbor.h"
#include "pr/p_norm.h"

std::unique_ptr<NearestNeighbor> generate_classifier( int argc, char ** argv ) {
    double tolerance = 0.1;
    bool euclidean = false;
    bool normalize = true;
    unsigned neighbors = 1;
    std::FILE * dataset_file = nullptr;

    static option options[] = {
        {"manhattan", no_argument, 0, 'm'},
        {"hamming", no_argument, 0, 'm'},
        {"euclidean", no_argument, 0, 'e'},
        {"neighbors", required_argument, 0, 'n'},
        {"normalize", no_argument, 0, 's'},
        {"no-normalize", no_argument, 0, 'r'},
        {"normalize-tolerance", required_argument, 0, 't'},
        {"dataset", required_argument, 0, 'd'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0},
    };
    int opt;
    int dummy_option_index;

    // Reset optind to not conflict with external getopt invocations.
    optind = 1;
    while( (opt = getopt_long( argc, argv, "men:srt:d:h",
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
            case 's':
                normalize = true;
                break;
            case 'r':
                normalize = false;
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
                std::printf( classifier_help_message, argv[0] );
                std::exit(0);
                break;
            default:
                std::fprintf( stderr, "Unknown parameter %c\n", optopt );
                std::exit(1);
        }
    }
    // Command line options parsed, now we will initialize the variables.
    std::unique_ptr<DataSet> dataset;
    std::unique_ptr<DistanceCalculator> calculator;

    if( dataset_file == nullptr ) {
        dataset = std::make_unique<DataSet>(DataSet::parse( stdin ));
    }
    else {
        dataset = std::make_unique<DataSet>(DataSet::parse( dataset_file ));
        std::fclose(dataset_file);
    }

    if( euclidean )
        calculator = std::make_unique<EuclideanDistance>( tolerance );
    else
        calculator = std::make_unique<ManhattanDistance>( tolerance );

    return std::make_unique<NearestNeighbor>(
        std::move(dataset),
        std::move(calculator),
        neighbors,
        normalize
    );
}
