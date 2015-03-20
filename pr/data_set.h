#ifndef DATA_SET_H
#define DATA_SET_H

#include <cstdio>
#include <string>
#include <vector>
#include "pr/data_entry.h"

class DataSet {
    std::vector< DataEntry > entries;
    std::vector< std::string > attribute_names;
    std::vector< std::string > category_names;

public:
    DataSet(
        std::vector< DataEntry >&& entries,
        std::vector< std::string >&& attribute_names,
        std::vector< std::string >&& category_names
    );

    static DataSet parse( std::FILE * source );

    const DataEntry * begin() const;
    const DataEntry * end() const;
    std::size_t size() const;
    std::size_t attribute_count() const;
    std::size_t category_count() const;
    std::string attribute_name( std::size_t index ) const;
    std::string category_name( std::size_t index ) const;
};

#endif // DATA_SET_H
