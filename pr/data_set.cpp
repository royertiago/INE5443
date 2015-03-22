#include "pr/data_set.h"

DataSet::DataSet(
    std::vector< std::string >&& attribute_names,
    std::vector< std::string >&& category_names,
    std::vector< DataEntry >&& entries
) :
    attribute_names(attribute_names),
    category_names(category_names),
    entries(entries)
{}

DataSet DataSet::parse( std::FILE * source ) {
    std::size_t field_count = 0;
    char c;

    while( std::fscanf( source, "%c", &c ) != EOF ) {
        if( c == '#' ) {
            std::fscanf( source, "%*[^\n]\n" );
            continue;
        }
        if( c == 'n' ) {
            std::fscanf( source, " %zd \n", &field_count );
            break;
        }
        throw "Unknown directive.";
    }
    if( field_count == 0 )
        throw "No fields in dataset.";

    std::vector< std::string > attribute_names;
    std::vector< std::string > category_names;
    std::string format;

    while( field_count-- ) {
        char type;
        std::fscanf( source, "%c", &type );
        format += type;
        std::fgetc(source); // Discard only first whitespace
        std::string name;
        while( (c = std::fgetc(source)) != '\n' )
            name += c;
        if( type == 'a' )
            attribute_names.push_back(name);
        else if( type == 'c' )
            category_names.push_back(name);
        else
            throw "Unknown field type.";
    }

    std::vector< DataEntry > entries;
    DataEntry entry;

    c = std::fgetc(source);
    std::ungetc(c, source);

    while( true ) {
        entry = DataEntry::parse( source, format.c_str() );
        if( entry.attribute_count() < attribute_names.size() ||
            entry.category_count() < category_names.size() )
            break;
        entries.push_back( entry );
    }

    return DataSet(
        std::move(attribute_names),
        std::move(category_names),
        std::move(entries)
    );
}

const DataEntry * DataSet::begin() const {
    return &*entries.begin();
}

const DataEntry * DataSet::end() const {
    return &*entries.end();
}

std::size_t DataSet::size() const {
    return entries.size();
}

std::size_t DataSet::attribute_count() const {
    return attribute_names.size();
}

std::size_t DataSet::category_count() const {
    return category_names.size();
}

std::string DataSet::attribute_name( std::size_t index ) const {
    return attribute_names[index];
}

std::string DataSet::category_name( std::size_t index ) const {
    return category_names[index];
}
