#include <cstdlib> // std::exit
#include <memory>
#include <iostream>
#include "classifier.h"
#include "cmdline/args.hpp"
#include "pr/data_set.h"
#include "pr/ibl.h"
#include "pr/nearest_neighbor.h"
#include "pr/p_norm.h"

std::unique_ptr<NearestNeighbor> generate_classifier( cmdline::args&& args ) {
    double tolerance = 0.1;
    bool euclidean = false;
    bool normalize = true;
    unsigned neighbors = 1;
    std::FILE * dataset_file = nullptr;

    int ibl = 0;
    bool shuffle = false;
    bool shuffle_seed_set = false;
    long long unsigned shuffle_seed;

    int noise = 0;
    long long unsigned noise_seed;
    bool noise_seed_set = false;
    double noise_expand = 0.1;

    double accept_threshold = 0.9;
    double reject_threshold = 0.75;
    long long unsigned ibl_seed;
    bool ibl_seed_set = false;

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
        if( arg == "--ibl" || arg == "--IBL" ) {
            args.range(1, 4) >> ibl;
            continue;
        }
        if( arg == "--shuffle" ) {
            shuffle = true;
            continue;
        }
        if( arg == "--shuffle-seed" ) {
            args >> shuffle_seed;
            shuffle = true;
            shuffle_seed_set = true;
            continue;
        }
        if( arg == "--noise" ) {
            args.range( 1 ) >> noise;
            continue;
        }
        if( arg == "--noise-seed" ) {
            args >> noise_seed;
            noise_seed_set = true;
            continue;
        }
        if( arg == "--noise-expand" ) {
            args >> noise_expand;
            continue;
        }
        if( arg == "--accept-threshold" ) {
            args.range( 0 ) >> accept_threshold;
            continue;
        }
        if( arg == "--reject-threshold" ) {
            args.range( 0 ) >> reject_threshold;
            continue;
        }
        if( arg == "--ibl-seed" || arg == "--IBL-seed" ) {
            args >> ibl_seed;
            ibl_seed_set = true;
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

    if( noise != 0 ) {
        if( noise_seed_set )
            dataset->noise( noise, noise_expand, noise_seed );
        else {
            long long unsigned seed = dataset->noise( noise, noise_expand );
            std::printf( "Noise seed: %llu\n", seed );
        }
    }

    if( shuffle ) {
        if( shuffle_seed_set )
            dataset->shuffle( shuffle_seed );
        else {
            auto seed = dataset->shuffle();
            std::cout << "Shuffle seed: " << seed << std::endl;
        }
    }

    if( ibl != 0 ) {
        std::unique_ptr<::ibl> ibl_ptr;
        switch( ibl ) {
            case 1:
                ibl_ptr = std::make_unique<ibl1>();
                break;
            case 2:
                ibl_ptr = std::make_unique<ibl2>();
                break;
            case 3:
                ibl_ptr = std::make_unique<ibl3>(accept_threshold, reject_threshold);
                if( false ) { // WARNING: entangled if and switch
            case 4:
                    ibl_ptr = std::make_unique<ibl3>(accept_threshold, reject_threshold);
                }
                { // begin common code
                    ibl3 * ibl3_ptr = (ibl3 *) ibl_ptr.get();
                    if( ibl_seed_set )
                        ibl3_ptr->seed( ibl_seed );
                    else
                        std::cout << "IBL seed: " << ibl3_ptr->seed() << std::endl;
                } // end common code
                break;
        }
        ibl_ptr->train( *dataset );
        std::cout << "Hits: " << ibl_ptr->hit_count()
            << " - Misses: " << ibl_ptr->miss_count() << "\n";
        dataset.reset( new DataSet(ibl_ptr->conceptual_descriptor()) );
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
