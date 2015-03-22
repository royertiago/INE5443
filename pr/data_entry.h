#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <cstdio>
#include <string>
#include <vector>
#include <initializer_list>

class DataEntry {
    std::vector< double > attributes;
    std::vector< std::string > categories;

public:
    DataEntry() = default;

    DataEntry( std::vector< double > &&, std::vector< std::string >&& );
    DataEntry( std::initializer_list<double>, std::initializer_list<const char*> );

    double attribute( std::size_t index ) const;
    const std::string& category( std::size_t index ) const;

    std::size_t attribute_count() const;
    std::size_t category_count() const;

    /* Parses a dummy entry from the specified file.
     * The entry will have the specified number of attributes,
     * but an empty category vector.
     *
     * If EOF is reached, a DataEntry
     * with less than size attributes is returned.
     *
     * The format is assumed to be comma-separated. */
    static DataEntry parse( std::FILE * file, std::size_t size );

    /* Parses an entry according to hte specified format.
     * If EOF is reached, a DataEntry
     * with less attributes of categories is returned. */
    static DataEntry parse( std::FILE * file, const char * format );

    /* Compare two DataEntries for equality.
     * The error tolerance for category_count is
     * std::numeric_limits<double>::epsilon(). */
    friend bool operator==( const DataEntry &, const DataEntry & );
    friend bool operator!=( const DataEntry &, const DataEntry & );
};

#endif // DATA_ENTRY_H
