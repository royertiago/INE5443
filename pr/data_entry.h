#ifndef DATA_ENTRY_H
#define DATA_ENTRY_H

#include <cstdio>
#include <iosfwd>
#include <string>
#include <vector>
#include <initializer_list>

/* class DataEntry
 *
 * This class represents a line in a dataset.
 * It contains both attributes and categories.
 *
 * Order of attributes is preserved,
 * as well as the order of categories;
 * however, no information about the order between attributes or categories
 * is kept in the object.
 */
class DataEntry {
    std::vector< double > attributes;
    std::vector< std::string > categories;
public:

    /* Constructs a new DataEntry from the specified vectors.
     * The first vector is the attribute vector,
     * the second vector is the category vector.
     *
     * Note that there is only one constructor taking vectors,
     * and they must be std::move'd to the object. */
    DataEntry( std::vector< double > &&, std::vector< std::string >&& );

    /* Construcs a new DataEntry from the specified initializer lists.
     * This function is meant to ease unit testing.
     *
     * Note the second initializer list is a list of char pointers,
     * not a list of strings. */
    DataEntry( std::initializer_list<double>, std::initializer_list<const char*> );

    /* Equivalent to DataEntry({},{}). */
    DataEntry() = default;

    /* Return the attribute or category in the chosen index,
     * respectively.
     *
     * No checking is done to assure that there is an attribute
     * or category in the specified index. */
    double attribute( std::size_t index ) const;
    const std::string& category( std::size_t index ) const;

    /* Return the number of attributes or categories
     * of this DataEntry, respectively. */
    std::size_t attribute_count() const;
    std::size_t category_count() const;

    /* Parses an entry according to the specified format.
     *
     * "format" is a string consisting entirely of 'a' and 'c'.
     * 'a' means to read an attribute;
     * 'c' means to read a category.
     * If EOF is reached, a DataEntry
     * with less attributes or categories is returned.
     *
     * Note that order of characters in 'format' matters.
     *
     * A trailing newline character after the last attribute or category
     * will be discarded. */
    static DataEntry parse( std::FILE * file, const char * format );

    /* Parses an entry with 'size' attributes
     * and no category.
     *
     * Equivalent to call parse( FILE*, const char * format )
     * with a string of 'size' characters 'a' in 'format'. */
    static DataEntry parse( std::FILE * file, std::size_t size );

    /* Compare two DataEntries for equality.
     * The error tolerance for category_count is
     * std::numeric_limits<double>::epsilon(). */
    friend bool operator==( const DataEntry &, const DataEntry & );
    friend bool operator!=( const DataEntry &, const DataEntry & );

    /* Prints a DataEntry.
     * Format example:
     * ({1.5,0.8},{"CategoryA"}) */
    friend std::ostream& operator<<( std::ostream& os, const DataEntry& );
};

#endif // DATA_ENTRY_H
