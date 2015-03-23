/* Utility program.
 * Strips out the categories of the DataEntry.
 *
 * To facilitate the implementation, the input format must be given
 * in the first argument. */
#include "pr/data_entry.h"

int main( int, char ** argv ) {
    DataEntry entry;
    while( (entry = DataEntry::parse(stdin, argv[1])).attribute_count() > 0 )
        DataEntry(std::vector<double>(entry.attributes()),{}).write(stdout);
    return 0;
}
