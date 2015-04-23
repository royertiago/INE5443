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
    DataSet() = default;

    /* Initializes an empty dataset with the given number
     * of attributes and categories, all of which with name "".
     */
    DataSet( std::size_t attribute_count, std::size_t category_count );

    static DataSet parse( std::FILE * source );

    /* Writes this dataset to the file.
     * 'format' is the order that attributes and categories
     * should appear in the file;
     * it have the same organization as DataEntry::write. */
    void write( std::FILE * file, const char * format = "" ) const;

    /* Appends the entry in the dataset.
     *
     * The given entry must have the correct number of attributes and categories.
     */
    void push_back( DataEntry&& );

    /* Shuffles the dataset.
     * The first version receives a seed to be used in random number generation.
     * The second version generates a seed, call the first version,
     * and returns the generated seed.
     */
    void shuffle( long long unsigned seed );
    long long unsigned shuffle();

    /* Entries that represent the minimum, maximum, and average values
     * for each attribute in the dataset.
     */
    DataEntry min() const;
    DataEntry max() const;
    DataEntry mean() const;

    /* Return a pair of normalizing vectors.
     * It is a pair of vectors, with attribute_count() elements each.
     * If an value v lies between min().attribute(i) and max().attribute(i),
     * then (v - pair.second[i]) / pair.first[i] will lie between 0 and 1.
     *
     * Note that if w is the value produced by the code above,
     * then v == w * pair.first[i] + pair.second[i].
     *
     * The expansion factor is used to virtually expand the dataset's extremum points.
     * FOr instance, if expand = 1.0, the dataset dimensions will be expanded
     * in 100% in every direction before mapping it to [0, 1].
     * So, the dataset will be mapped to the interval [1/3, 2/3] instead.
     * Choose expand == 0.0 to disable this feature.
     */
    std::pair<std::vector<double>, std::vector<double>>
    normalizing_factor( double expand ) const;

    /* Generate a copy of this dataset, but only with the header
     * (attribute names and category names).
     */
    DataSet header() const;

    const DataEntry * begin() const;
    const DataEntry * end() const;
    std::size_t size() const;
    std::size_t attribute_count() const;
    std::size_t category_count() const;
    std::string attribute_name( std::size_t index ) const;
    std::string category_name( std::size_t index ) const;
};

#endif // DATA_SET_H
