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

    /* Generates some points of noise in the dataset.
     *
     * The noise is first generated in an hypercube,
     * using an uniform random number generator,
     * and then the cube is scaled to fit the database.
     *
     * (The scaling is done dimension-wise; the resulting shape
     * most likely is not a cube anymore.)
     *
     * Additionaly, you can specify the value `expand`,
     * which will expand the cube a bit after the fitting.
     * This has the effect of generating noise
     * outside the original dataset boundaries.
     * For instance, with an `expand` value of 0.5,
     * a dimension that ranges from 0 to 2 in the database
     * might receive random values from -1 to 3 by this method.
     *
     * The categories are also generated randomly, but weighted
     * by the current categories. No new category will be added.
     *
     * Akin to DataSet::shuffle, the first version accepts a random seed
     * and the second generates one and returns it.
     */
    void noise( std::size_t number_of_points, double expand, long long unsigned seed );
    long long unsigned noise( std::size_t number_of_points, double expand );

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

    /* This method normalizes every coordinate to the interval [x, 1-x],
     * where x = expand / (2 * expand + 1).
     *
     * Intervals for some example values of `expand`:
     *  0.0: [0, 1]
     *  0.5: [1/4, 3/4]
     *  1.0: [1/3, 2/3]
     *
     * This function effectively applies the transformation
     * returned by normalizing_factor.
     */
    void normalize( double expand );

    /* Return a pair of standardizing vectors.
     * These vectors are similar in spirit to the vectors returned by
     * data_set::normalizing_factor,
     * but instead of normalizing each attribute to the interval [0, 1],
     * the output will be in terms of standard deviations.
     * That is, an output of s for the input v of the attribute i
     * means that v is s standard deviations distantant
     * from the mean of the attribute i.
     *
     * We ignore Bessel's correction.
     */
    std::pair<std::vector<double>, std::vector<double>> standardize_factor() const;

    /* Rewrite each point in the database
     * in terms of standard deviations from the mean.
     *
     * This function effectively applies the transformation
     * returned by standardize_factor.
     */
    void standardize();

    /* Return counting data about the categories of this dataset.
     * If m is the returned value, then
     * m[i][j].first is the jth category (in alphabetical order)
     * if the ith category. There are m[i][j].second entries in the set
     * that have the category m[i][j].first in the position i.
     */
    std::vector<std::vector<std::pair<std::string, std::size_t>>>
    category_statistics() const;

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
