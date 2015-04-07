#ifndef PR_GRID_GENERATOR_H
#define PR_GRID_GENERATOR_H

/* Generates an uniform list of entries.
 */

#include <vector>
#include "pr/data_entry.h"
#include "pr/data_set.h"

class GridGenerator {
    double _expand = 0.1;
    std::vector< unsigned > _density;
    std::vector< double > step;
    std::vector< double > shift;

    static constexpr unsigned default_density = 10;
public:

    /* Sets/gets the expansion factor;
     * how much the grid should be expanded
     * relatively to the extremum points of the dataset.
     *
     * Default: 0.1.
     */
    void expand( double );
    const double & expand() const;

    /* Sets/gets how much points should be generated in every dimension.
     *
     * Default: 100 per dimension.
     */
    void density( const std::vector<unsigned> & );
    const std::vector<unsigned>& density() const;

    /* Adjust the density of the grid to the choosen dataset
     * and the specified values of density and expansion factor.
     */
    void calibrate( const DataSet & );

    /* Obtains the point in the grid that corresponds to
     * the passed index vector.
     */
    DataEntry operator()( const std::vector<unsigned> & ) const;

    /* Iterator that generates all grid entries.
     * The iterator assumes the underlying generator
     * to be untouched during its operation.
     */
    class GridIterator {
        std::vector<unsigned> index;
        bool expired;
        const GridGenerator * grid;

        friend class GridGenerator;
    public:

        DataEntry operator*() const;
        GridIterator & operator++();
        GridIterator operator++(int) {
            auto it = *this;
            ++*this;
            return it;
        }

        friend bool operator==( const GridIterator &, const GridIterator & );
        friend bool operator!=( const GridIterator &, const GridIterator & );
    };

    GridIterator begin() const;
    GridIterator end() const;
};

#endif // PR_GRID_GENERATOR_H
