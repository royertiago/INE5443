#include <algorithm>
#include <cfloat>
#include <chrono>
#include <cmath>
#include <map>
#include <random>
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
        else if( type == 'i' )
            ; // no-op
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
    bool has_name = entries.front().name() != "";
    std::fprintf( file, "n %zd\n",
            attribute_names.size() + category_names.size() + has_name );
    const char * original_format = format;

    auto attribute_it = attribute_names.begin();
    auto category_it = category_names.begin();
    bool name_printed = false;
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
        else if( *format == 'i' ) {
            if( name_printed )
                throw "Too much 'i' specifiers.";
            std::fprintf( file, "i \n" );
            name_printed = true;
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
    if( has_name && !name_printed && !entries.empty() )
        std::fprintf( file, "i \n" );
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

void DataSet::shuffle( long long unsigned seed ) {
    std::mt19937 rng(seed);
    std::shuffle( entries.begin(), entries.end(), rng );
}

long long unsigned DataSet::shuffle() {
    long long unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    shuffle(seed);
    return seed;
}

void DataSet::noise( std::size_t number, double expand, long long unsigned seed ) {
    std::mt19937 rng(seed);

    auto stat = category_statistics();
    std::vector< std::discrete_distribution<> > discrete_distributions;
    for( unsigned i = 0; i < stat.size(); i++ ) {
        std::vector<int> weights(stat[i].size());
        for( unsigned j = 0; j < stat[i].size(); j++ )
            weights[j] = stat[i][j].second;
        discrete_distributions.push_back(
            std::discrete_distribution<>(weights.begin(), weights.end())
        );
    }
    auto category = [&]( std::size_t index ) {
        return stat[index][
            discrete_distributions[index](rng)
        ].first;
    };

    auto min = this->min();
    auto max = this->max();
    std::vector< std::uniform_real_distribution<> > real_distributions;
    for( unsigned i = 0; i < min.attribute_count(); i++ ) {
        double expand_factor = (max.attribute(i) - min.attribute(i)) * expand;
        real_distributions.push_back( std::uniform_real_distribution<>(
            min.attribute(i) - expand_factor,
            max.attribute(i) + expand_factor
        ) );
    }
    auto attribute = [&]( std::size_t index ) {
        return real_distributions[index](rng);
    };

    for( std::size_t c = 0; c < number; c++ ) {
        std::vector<double> attributes( attribute_count() );
        std::vector<std::string> categories( category_count() );
        for( unsigned i = 0; i < attribute_count(); i++ )
            attributes[i] = attribute(i);
        for( unsigned i = 0; i < category_count(); i++ )
            categories[i] = category(i);
        push_back( DataEntry(std::move(attributes), std::move(categories)) );
    }
}

long long unsigned DataSet::noise( std::size_t number, double expand ) {
    long long unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    noise( number, expand, seed );
    return seed;
}

DataEntry DataSet::min() const {
    auto minimum_value = std::vector<double>( attribute_count(), DBL_MAX );
    for( const DataEntry & entry: *this )
        for( int i = 0; i < attribute_count(); i++ )
            minimum_value[i] = std::min(minimum_value[i], entry.attribute(i));
    return DataEntry( std::move(minimum_value), std::vector<std::string>() );
}

DataEntry DataSet::max() const {
    auto maximum_value = std::vector<double>( attribute_count(), -DBL_MAX );
    for( const DataEntry & entry: *this )
        for( int i = 0; i < attribute_count(); i++ )
            maximum_value[i] = std::max(maximum_value[i], entry.attribute(i));
    return DataEntry( std::move(maximum_value), std::vector<std::string>() );
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

std::pair<std::vector<double>, std::vector<double>>
DataSet::normalizing_factor( double expand ) const {
    auto minimum_value = min().attributes();
    auto maximum_value = max().attributes();
    std::vector< double > multiplicative_factor(attribute_count());

    for( std::size_t i = 0; i < attribute_count(); i++ ) {
        double distance = maximum_value[i] - minimum_value[i];
        double expansion_radius = expand * distance;
        maximum_value[i] += expansion_radius;
        minimum_value[i] -= expansion_radius;
        multiplicative_factor[i] = 1/(maximum_value[i] - minimum_value[i]);
    }
    return std::make_pair(std::move(multiplicative_factor), std::move(minimum_value));
}

void DataSet::normalize( double factor ) {
    auto pair = normalizing_factor( factor );
    for( auto & entry : entries )
        for( std::size_t i = 0; i < category_count(); i++ )
            entry.attribute(i) = ( entry.attribute(i) - pair.second[i] )*pair.first[i];
}

std::pair<std::vector<double>, std::vector<double>>
DataSet::standardize_factor() const {
    auto mean = this->mean().attributes();
    std::vector< double > variance(attribute_count());

    auto square = []( double d ){ return d * d; };

    for( const DataEntry & e : entries )
        for( std::size_t i = 0; i < attribute_count(); i++ )
            variance[i] += square( mean[i] - e.attribute(i) );

    for( double & d : variance )
        d = std::sqrt( entries.size() / d );

    return std::make_pair( std::move(variance), std::move(mean) );
}

void DataSet::standardize() {
    auto pair = standardize_factor();
    for( auto & entry : entries )
        for( std::size_t i = 0; i < category_count(); i++ )
            entry.attribute(i) = ( entry.attribute(i) - pair.second[i] )*pair.first[i];
}

std::vector<std::vector<std::pair<std::string, std::size_t>>>
DataSet::category_statistics() const {
    std::map< std::pair<std::size_t, std::string>, std::size_t > stat;
    /* stat[{i, str}] == j means there is j elements in the dataset
     * with str as its ith category.
     */
    for( const auto & entry : *this )
        for( unsigned i = 0; i < entry.category_count(); i++ )
            stat[std::make_pair(i, entry.category(i))]++;

    std::vector<std::vector<std::pair<std::string, std::size_t>>> ret(category_count());
    for( const auto & pair : stat )
        ret[pair.first.first].push_back(std::make_pair(pair.first.second, pair.second));

    return ret;
}

DataSet DataSet::header() const {
    return DataSet(
        std::vector<std::string>(attribute_names),
        std::vector<std::string>(category_names),
        std::vector<DataEntry>()
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
