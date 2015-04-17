#include <cstdlib> // std::exit
#include <memory>
#include <iostream>
#include "classifier.h"
#include "cmdline/args.h"
#include "pr/data_set.h"
#include "pr/nearest_neighbor.h"
#include "pr/p_norm.h"

std::unique_ptr<NearestNeighbor> generate_classifier( cmdline::args&& args ) {
    double tolerance = 0.1;
    bool euclidean = false;
    bool normalize = true;
    unsigned neighbors = 1;
    std::FILE * dataset_file = nullptr;

    while( args.size() > 0 ) {
        std::string arg = args.next();
        if( arg == "--manhattan" || arg == "--hamming" ) {
            euclidean = false;
            continue;
        }
        if( arg == "--euclidean" ) {
            euclidean = true;
            continue;
        }
        if( arg == "--neighbors" ) {
            args >> neighbors;
            if( neighbors <= 0 ) {
                std::cerr << "Number of neighbors must be positive.\n";
                std::exit(1);
            }
            continue;
        }
        if( arg == "--normalize" ) {
            normalize = true;
            continue;
        }
        if( arg == "--no-normalize" ) {
            normalize = false;
            continue;
        }
        if( arg == "--normalize-tolerance" ) {
            args >> tolerance;
            if( tolerance < 0 ) {
                std::cerr << "Normalizing tolerance must be positive.\n";
                std::exit(1);
            }
            continue;
        }
        if( arg == "--dataset" ) {
            std::string filename = args.next();
            dataset_file = std::fopen( filename.c_str(), "r" );
            if( dataset_file == nullptr ) {
                std::cerr << "Could not open file " << filename << '\n';
                std::exit(1);
            }
            continue;
        }
        if( arg == "--help" ) {
            std::cout << args.program_name() << classifier_help_message;
            std::exit(0);
            continue;
        }
        std::cerr << args.program_name() << ": Unknown option " << arg << '\n';
        std::exit(1);
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
