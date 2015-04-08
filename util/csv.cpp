#include "csv.h"

namespace util {

    std::vector<std::vector<int>> parse_csv( std::FILE* file ) {
        std::vector<std::vector<int>> data;
        char c;
        std::vector<int> current;
        while( (c = std::fgetc(file)) != EOF ) {
            if( c == '\n' ) {
                data.push_back( current );
                current.clear();
                continue;
            }
            else if( c != ',' )
                std::ungetc( c, file );

            int i;
            if( std::fscanf( file, "%i", &i ) == 1 )
                current.push_back(i);
            else
                throw "Incorrect input fornat.";
        }

        return data;
    }

} // namespace util
