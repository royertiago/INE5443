#ifndef DATA_SET_H
#define DATA_SET_H

#include <cstdio>
#include <string>
#include <vector>
#include "pr/data_entry.h"

class DataSet {
    std::vector< std::string > attribute_names;
    std::vector< std::string > category_names;
    std::vector< DataEntry > entries;

public:
    DataSet(
        std::vector< std::string >&& attribute_names,
        std::vector< std::string >&& category_names,
        std::vector< DataEntry >&& entries
    );

    static DataSet parse( std::FILE * source );

    /* Writes this dataset to the file.
     * 'format' is the order that attributes and categories
     * should appear in the file;
     * it have the same organization as DataEntry::write. */
    void write( std::FILE * file, const char * format = "" ) const;

    const DataEntry * begin() const;
    const DataEntry * end() const;
    std::size_t size() const;
    std::size_t attribute_count() const;
    std::size_t category_count() const;
    std::string attribute_name( std::size_t index ) const;
    std::string category_name( std::size_t index ) const;
};

#endif // DATA_SET_H
