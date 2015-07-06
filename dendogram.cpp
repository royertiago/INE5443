namespace command_line {
    const char help_message[] =
" [options]\n"
"Constructs and shows a dendogram from the dataset read in stdin.\n"
"Categories are ignored; the name is used to identify each entry\n"
"in the dendogram leaves.\n"
"\n"
"Optionally, analyses the dendogram to find an optimum number of classes.\n"
"The new dataset, with all categories replaced\n"
"by the ones generated from the dendogram, is written to stdout.\n"
"\n"
"Options:\n"
"--simple\n"
"--full\n"
"--mean\n"
"    Chooses between simple linkage, full linkage or mean (average) linkage.\n"
"    Default: simple.\n"
"\n"
"--width <N>\n"
"--height <N>\n"
"    Choose the dimensions of the image.\n"
"    Default value: 800 and 600.\n"
"\n"
"--nonames\n"
"    Do not write the name of the data entries next to the dendogram leaves.\n"
"    Note that the names will be empty if there is no names in the dataset;\n"
"    this option may be useful in this case.\n"
"    Default: write the names.\n"
"\n"
"--noshow\n"
"    Disable image generation altogether.\n"
"    Default: show image.\n"
"\n"
"--output <file>\n"
"    Write the generated image to <file>.\n"
"    No writing is done by default.\n"
"\n"
"--min-class <N>\n"
"--max-class <N>\n"
"    Activates the analyzer with the specified limitations.\n"
"    The number of classes will be between min-class and max-class, inclusive.\n"
"    Default: do not perform analysis.\n"
"\n"
"--showsplit\n"
"    Displays a red line in the splitting point\n"
"    (the point where the dendogram was cut after the analysis).\n"
"    Avaliable only when performing the analysis.\n"
"\n"
"--showlimits\n"
"    Displays two gray lines in the limits min_class and max_class.\n"
"    Avaliable only when performing the analysis.\n"
"\n"
"--classes\n"
"    Writes to stdout only the number of classes.\n"
"    Default: writes the new dataset to stdout.\n"
"    Avaliable only when performing the analysis.\n"
"\n"
"--help\n"
"    Display this help and exit.\n"
"\n";
} // namespace command_line

#include <cstdio>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "cmdline/args.hpp"
#include "pr/data_set.h"
#include "pr/dendogram_node.h"
#include "pr/dendogram.h"
#include "util/cv.h"

namespace command_line {
    int width = 800;
    int height = 600;

    bool show_image = true;
    bool print_names = true;

    bool analyse = false;
    int min_class = -1;
    int max_class = -1;
    bool show_split = false;
    bool show_limits = false;
    bool write_classes = false;

    std::string output_file_name;

    LinkageDistanceFunction * linkage = SimpleLinkage;
    LinkageDistanceUpdateFunction * update = SimpleLinkageUpdate;

    void parse( cmdline::args&& args ) {
        while( args.size() > 0 ) {
            std::string arg = args.next();
            if( arg == "--simple" ) {
                linkage = SimpleLinkage;
                continue;
            }
            if( arg == "--full" ) {
                linkage = FullLinkage;
                update = FullLinkageUpdate;
                continue;
            }
            if( arg == "--mean" ) {
                linkage = MeanLinkage;
                update = MeanLinkageUpdate;
                continue;
            }
            if( arg == "--width" ) {
                args.range(1) >> width;
                continue;
            }
            if( arg == "--height" ) {
                args.range(1) >> height;
                continue;
            }
            if( arg == "--nonames" ) {
                print_names = false;
                continue;
            }
            if( arg == "--noshow" ) {
                show_image = false;
                continue;
            }
            if( arg == "--output" ) {
                output_file_name = args.next();
                continue;
            }
            if( arg == "--min-class" ) {
                args.range(1) >> min_class;
                analyse = true;
                continue;
            }
            if( arg == "--max-class" ) {
                args.range(1) >> max_class;
                analyse = true;
                continue;
            }
            if( arg == "--showsplit" ) {
                show_split = true;
                continue;
            }
            if( arg == "--showlimits" ) {
                show_limits = true;
                continue;
            }
            if( arg == "--classes" ) {
                write_classes = true;
                continue;
            }
            if( arg == "--help" ) {
                std::cout << args.program_name() << help_message;
                std::exit(0);
            }
            std::cerr << args.program_name() << ": Unknown option " << arg << '\n';
            std::exit(1);
        }
        if( analyse ) {
            if( min_class == -1 ) {
                std::cerr << "Must supply --min-class also to perform analysis.\n";
                std::exit(1);
            }
            if( max_class == -1 ) {
                std::cerr << "Must supply --max-class also to perform analysis.\n";
                std::exit(1);
            }
        }
        else {
            if( show_split ) {
                std::cerr << "--showsplit only avaliable when analysing.\n";
                std::exit(1);
            }
            if( show_limits ) {
                std::cerr << "--showlimits only avaliable when analysing.\n";
                std::exit(1);
            }
            if( write_classes ) {
                std::cerr << "--classes only avaliable when analysing.\n";
                std::exit(1);
            }
        }
    }
} // namespace command_line

int main( int argc, char ** argv ) {
    command_line::parse( cmdline::args(argc, argv) );

    DataSet dataset = DataSet::parse( stdin );
    auto dendogram = generate_dendogram(
        dataset,
        command_line::linkage,
        command_line::update
    );

    cv::Mat img(
        command_line::height, command_line::width,
        CV_8UC3, cv::Scalar(255, 255, 255)
    );

    int word_width;

    if( command_line::show_image || command_line::output_file_name != "" ) {
        if( command_line::print_names )
            word_width = util::print_named_dendogram( img, *dendogram );
        else {
            word_width = 0;
            util::print_dendogram( img, *dendogram );
        }
    }

    if( command_line::output_file_name != "" ) {
        if( !cv::imwrite( command_line::output_file_name, img ) )
            std::fprintf( stderr, "Error writing image to %s\n",
                command_line::output_file_name.c_str()
            );
    }

    if( command_line::analyse ) {
        auto data = classify_dendogram(
            *dendogram,
            command_line::min_class,
            command_line::max_class,
            dataset
        );
        if( command_line::write_classes )
            std::printf( "Optimum number of classes: %d\n", data.classes );
        else
            data.classified_dataset.write( stdout );

        int remaining_width = command_line::width - word_width;

        if( command_line::show_limits ) {
            int pos = data.linkage_min_class / dendogram->linkage_distance() *
                remaining_width + word_width;
            cv::line(
                img,
                cv::Point(pos, 0), cv::Point(pos, command_line::height),
                cv::Scalar(128,128,128)
            );

            pos = data.linkage_max_class / dendogram->linkage_distance() *
                remaining_width + word_width;
            cv::line(
                img,
                cv::Point(pos, 0), cv::Point(pos, command_line::height),
                cv::Scalar(128,128,128)
            );
        }

        if( command_line::show_split ) {
            int pos = (data.linkage_lower_limit + data.linkage_upper_limit) /
                (2 * dendogram->linkage_distance()) * remaining_width + word_width;
            cv::line(
                img,
                cv::Point(pos, 0), cv::Point(pos, command_line::height),
                cv::Scalar(0,0,255)
            );
        }
    } // if( command_line::analyse )

    if( command_line::show_image ) {
        cv::imshow( "Dendogram", img );
        cv::waitKey();
    }

    return 0;
}
