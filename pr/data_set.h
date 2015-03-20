#ifndef DATA_SET_H
#define DATA_SET_H

#include <stdio.h>
#include <vector>
#include "pr/data_entry.h"

class DataSet {
    std::vector< DataEntry > entries;

public:
    explicit DataSet( const std::vector< DataEntry >& entries );
    explicit DataSet( std::vector< DataEntry >&& entries );
    static DataSet parse( FILE * source );

    const DataEntry * begin() const;
    const DataEntry * end() const;
    std::size_t size() const;
    std::size_t attribute_count() const;
    std::size_t category_count() const;
};

#endif // DATA_SET_H
