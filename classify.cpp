/* Driver program for the classifier.
 */
#include "pr/classifier.h"

int main( int argc, char ** argv ) {
    Classifier classifier(argc, argv);
    std::size_t attribute_count = classifier.dataset_attribute_count();
    DataEntry entry;
    while( true ) {
        entry = DataEntry::parse(stdin, attribute_count );
        if( entry.attribute_count() != attribute_count )
            break;

        DataEntry({},classifier.classify(entry)).write( stdout );
    }
    return 0;
}
