#include <utility>
#include "data_entry.h"

DataEntry::DataEntry( std::vector< double > && a, std::vector< std::string >&& c ) :
    attributes( a ),
    categories( c )
{}

double DataEntry::attribute( std::size_t index ) const {
    return attributes[index];
}

const std::string& DataEntry::category( std::size_t index ) const {
    return categories[index];
}

std::size_t DataEntry::attribute_count() const {
    return attributes.size();
}

std::size_t DataEntry::category_count() const {
    return categories.size();
}

DataEntry DataEntry::parse( std::FILE * file, std::size_t size ) {
    if( size == 0 )
        return DataEntry( std::vector<double>(), std::vector< std::string >() );

    std::vector< double > attributes;
    double current_attribute;
    std::fscanf( file, "%lf", &current_attribute );
    attributes.push_back(current_attribute);

    while( --size ) {
        std::fscanf( file, " ,%lf", &current_attribute );
        attributes.push_back(current_attribute);
    }
    return DataEntry( std::move(attributes), std::vector<std::string>() );
}

DataEntry DataEntry::parse( std::FILE * file, const char * format ) {
    std::vector< double > attributes;
    std::vector< std::string > categories;
    while( *format != '\0' ) {
        if( *format == 'a' ) {
            double current_attribute;
            std::fscanf( file, "%lf", &current_attribute );
            attributes.push_back(current_attribute);
        }
        else if( *format == 'c' ) {
            std::string name;
            char c = fgetc( file );
            while( c != '\n' && c != ',' ) {
                name += c;
                c = fgetc(file);
            }
            categories.push_back(name);
        }
        else
            throw "Unknown format.";
    }
    return DataEntry( std::move(attributes), std::move(categories) );
}
