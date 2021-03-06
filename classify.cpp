/* Driver program for the classifier.
 *
 * The list of command line arguments is avaliable in pr/classifier.h.
 */
#include "pr/classifier.h"
#include "pr/data_set.h"

int main( int argc, char ** argv ) {
    auto ptr = generate_classifier(cmdline::args(argc, argv));
    NearestNeighbor & classifier = *ptr;
    std::size_t attribute_count = classifier.dataset().attribute_count();
    DataEntry entry;
    while( true ) {
        entry = DataEntry::parse(stdin, attribute_count );
        if( entry.attribute_count() != attribute_count )
            break;

        DataEntry({},classifier.classify(entry)).write( stdout );
    }
    return 0;
}
