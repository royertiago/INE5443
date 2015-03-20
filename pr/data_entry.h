#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <cstdio>
#include <string>
#include <vector>

class DataEntry {
    std::vector< double > attributes;
    std::vector< std::string > categories;

public:
    DataEntry( std::vector< double > &&, std::vector< std::string >&& );

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
    static DataEntry parseEntry( std::FILE * file, std::size_t size );
};

#endif // DATA_ENTRY_H
