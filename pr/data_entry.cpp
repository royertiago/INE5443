#include <cmath>
#include <limits>
#include <ostream>
#include <utility>
#include "data_entry.h"

DataEntry::DataEntry( std::vector< double > && a, std::vector< std::string >&& c ) :
    attributes( a ),
    categories( c )
{}

DataEntry::DataEntry( std::initializer_list< double > attributes,
                      std::initializer_list< const char * > categories ) :
    attributes( attributes.begin(), attributes.end() ),
    categories( categories.begin(), categories.end() )
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

// Public operators implementation
bool operator==( const DataEntry & lhs, const DataEntry & rhs ) {
    if( lhs.attribute_count() != rhs.attribute_count() )
        return false;

    for( std::size_t i = 0; i < lhs.attribute_count(); i++ )
        if( std::fabs(lhs.attribute(i) - rhs.attribute(i))
                > std::numeric_limits<double>::epsilon() )
            return false;

    return lhs.categories == rhs.categories;
}

bool operator!=( const DataEntry & lhs, const DataEntry & rhs ) {
    return !(lhs == rhs);
}

std::ostream & operator<<( std::ostream & os, const DataEntry & rhs ) {
    const char * separator = "";

    os << "({";
    for( const auto & a: rhs.attributes ) {
        os << separator << a;
        separator = ",";
    }
    os << "},{";
    separator = "";
    for( const auto & c: rhs.categories ) {
        os << separator << c;
        separator = ",";
    }
    os << "})";
    return os;
}
