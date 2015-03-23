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
    return std::move( DataEntry::parse(file, std::string( size, 'a' ).c_str()) );
}

DataEntry DataEntry::parse( std::FILE * file, const char * format ) {
    std::vector< double > attributes;
    std::vector< std::string > categories;
    int c;

    while( *format != '\0' ) {
        if( *format == 'a' ) {
            double current_attribute;
            if( std::fscanf( file, "%lf", &current_attribute ) == EOF )
                goto end_of_file;
            attributes.push_back(current_attribute);
        }
        else if( *format == 'c' ) {
            std::string name;
            c = std::fgetc( file );
            if( c == EOF ) goto end_of_file;
            while( c != '\n' && c != ',' ) {
                name += c;
                c = std::fgetc(file);
                if( c == EOF ) goto end_of_file;
            }
            if( c == ',' )
                std::ungetc(',', file);
            categories.push_back(name);
        }
        else
            throw "Unknown format.";

        ++format;
        if( *format != '\0' ) {
            c = std::fgetc( file );
            if( c == EOF ) goto end_of_file;
            if( c != ',' )
                throw "Input format is missing a comma.";
        }
    }

    /* Discard a trailing newline */
    c = std::fgetc( file );
    if( c != '\n' )
        std::ungetc( c, file );

end_of_file:
    return DataEntry( std::move(attributes), std::move(categories) );
}

void DataEntry::write( std::FILE * file, const char * format ) const {
    auto attribute_it = attributes.begin();
    auto category_it = categories.begin();
    const char * separator = "";
    while( *format != '\0' ) {
        if( *format == 'a' ) {
            if( attribute_it == attributes.end() )
                throw "Too much 'a' specifiers.";
            std::fprintf( file, "%s%lf", separator, *attribute_it );
            separator = ",";
            ++attribute_it;
        }
        else if( *format == 'c' ) {
            if( category_it == categories.end() )
                throw "Too much 'c' specifiers.";
            std::fprintf( file, "%s%s", separator, category_it->c_str() );
            separator = ",";
            ++category_it;
        }
        else
            throw "Unknown specifier.";
        ++format;
    }
    while( attribute_it != attributes.end() ) {
        std::fprintf( file, "%s%lf", separator, *attribute_it );
        separator = ",";
        ++attribute_it;
    }
    while( category_it != categories.end() ) {
        std::fprintf( file, "%s%s", separator, category_it->c_str() );
        separator = ",";
        ++category_it;
    }
    std::fprintf( file, "\n" );
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
