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

DataSet::DataSet( std::size_t attribute_count, std::size_t category_count ) :
    attribute_names(attribute_count),
    category_names(category_count),
    entries()
{}

DataSet DataSet::parse( std::FILE * source ) {
    std::size_t field_count = 0;
    int c;

    while( (c = std::fgetc(source)) != EOF ) {
        if( c == '#' ) {
            while( c != '\n' )
                c = std::fgetc(source);
            continue;
        }
        if( c == 'n' ) {
            std::fscanf( source, " %zd ", &field_count );
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

void DataSet::write( std::FILE * file, const char * format ) const {
    std::fprintf( file, "n %zd\n", attribute_names.size() + category_names.size() );
    const char * original_format = format;

    auto attribute_it = attribute_names.begin();
    auto category_it = category_names.begin();
    while( *format != '\0' ) {
        if( *format == 'a' ) {
            if( attribute_it == attribute_names.end() )
                throw "Too much 'a' specifiers.";
            std::fprintf( file, "a %s\n", attribute_it->c_str() );
            ++attribute_it;
        }
        else if( *format == 'c' ) {
            if( category_it == category_names.end() )
                throw "Too much 'c' specifiers.";
            std::fprintf( file, "c %s\n", category_it->c_str() );
            ++category_it;
        }
        else
            throw "Unknown specifier.";
        ++format;
    }
    while( attribute_it != attribute_names.end() ) {
        std::fprintf( file, "a %s\n", attribute_it->c_str() );
        ++attribute_it;
    }
    while( category_it != category_names.end() ) {
        std::fprintf( file, "c %s\n", category_it->c_str() );
        ++category_it;
    }
    std::fprintf( file, "\n" );
    for( const auto & entry : entries )
        entry.write( file, original_format );
}

void DataSet::push_back( DataEntry && entry ) {
    if( entry.attribute_count() == attribute_count() &&
        entry.category_count() == category_count()
    )
        entries.push_back( std::move(entry) );
    else
        throw "Wrong number of attributes or categories.";
}

DataEntry DataSet::mean() const {
    std::vector<double> sum( attribute_count() );
    for( const DataEntry & entry: *this )
        for( int i = 0; i < attribute_count(); i++ )
            sum[i] += entry.attribute(i);

    for( int i = 0; i < attribute_count(); i++ )
        sum[i] /= size();

    return DataEntry( std::move(sum), std::vector<std::string>() );
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
