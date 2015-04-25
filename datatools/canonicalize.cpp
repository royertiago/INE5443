const char help_message[] =
"%s < dataset > normalized\n"
"Rewrites the dataset in the stdin to stdout\n"
"so that every attribute comes before every category.\n"
"\n"
"Options:\n"
"--help\n"
"    Display this help and quit.\n"
;
#include <cstdio>
#include <cstring>
#include "pr/data_set.h"

int main( int argc, char ** argv ) {
    if( argc == 1 ) {
        DataSet dataset = DataSet::parse( stdin );
        dataset.write( stdout );
        return 0;
    }
    std::FILE * output = stderr;
    int ret_val = 1;
    if( argc == 2 && std::strcmp(argv[1], "--help") == 0 ) {
        output = stdout;
        ret_val = 0;
    }

    std::fprintf( output, help_message, argv[0] );
    return ret_val;
}
