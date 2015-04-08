#ifndef UTIL_CSV_PARSE_H
#define UTIL_CSV_PARSE_H

#include <cstdio>
#include <vector>

namespace util {

    /* Parses a comma-separated integer list from the specified file.
     * Each line of the file will be treated as an entry
     * and will have its own vector in the output.
     */
    std::vector<std::vector<int>> parse_csv( std::FILE* file );

} // namespace util

#endif // UTIL_CSV_PARSE_H
