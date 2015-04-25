const char help_message[] =
"%s format\n"
"Strips out the categories of the DataEntry.\n"
"Entries are read from stdin; the input format must be given.\n"
"Output is written to stdout.\n"
"\n"
"The sprecial value \"--help\" for format shows this help and quit.\n"
;
#include <cstdio>
#include <cstring>
#include "pr/data_entry.h"

int main( int argc, char ** argv ) {
    if( argc != 2 ) {
        std::fprintf( stderr, help_message, argv[0] );
        std::exit(1);
    }
    if( std::strcmp(argv[1], "--help") == 0 ) {
        std::printf( help_message, argv[0] );
        return 0;
    }

    DataEntry entry;
    while( (entry = DataEntry::parse(stdin, argv[1])).attribute_count() > 0 )
        DataEntry(std::vector<double>(entry.attributes()),{}).write(stdout);
    return 0;
}
