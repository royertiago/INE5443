#include "data_entry.h"

DataEntry::DataEntry( std::vector< double > && a, std::vector< std::string >&& c ) :
    attributes( a ),
    categories( c )
{}

double DataEntry::attribute( std::size_t index ) const {
    return attributes[index];
}

const std::string& DataEntry::category( std::size_t index ) const {
    return category[index];
}

std::size_t DataEntry::attribute_count() const {
    return attributes.size();
}

std::size_t DataEntry::category_count() const {
    return categories.size();
}

static DataEntry DataEntry::parseEntry( std::FILE * file, std::size_t size ) {
    if( size == 0 )
        return DataEntry( std::vector<double>(), std::vector<std::string>>() );

    std::vector< double > attributes;
    double current_attribute;
    std::fscanf( file, "%lf", &current_attribute );
    attributes.push_back(current_attribute);

    while( --size ) {
        double current_attribute;
        std::fscanf( file, " ,%lf", &current_attribute );
        attributes.push_back(current_attribute);
    }
    return DataEntry( attributes, std::vector<std::string>() );
}
