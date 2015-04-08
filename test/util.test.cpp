#include "util/csv.h"
#include <catch.hpp>

TEST_CASE( "Comma-separated value parsing", "[csv][parse][util]" ) {

    SECTION( "Normal values" ) {
        char str_file[] =
            "1,2\n"
            "2,8\n"
            "3,0\n"
            ;
        std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

        CHECK( util::parse_csv(file) ==
            (std::vector<std::vector<int>>{
                {1, 2},
                {2, 8},
                {3, 0},
            }));
        CHECK( std::fgetc(file) == EOF );
    }

    SECTION( "Negative values" ) {
        char str_file[] =
            "1,2,-9\n"
            "2,8,-8\n"
            ;
        std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

        CHECK( util::parse_csv(file) ==
            (std::vector<std::vector<int>>{
                {1, 2,-9},
                {2, 8,-8},
            }));
        CHECK( std::fgetc(file) == EOF );
    }

    SECTION( "Inconsistent sizes" ) {
        char str_file[] =
            "1,2,3,4,5,6,7,8,9\n"
            "1,2\n"
            "2,8,-8\n"
            "0\n"
            ;
        std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

        CHECK( util::parse_csv(file) ==
            (std::vector<std::vector<int>>{
                {1, 2, 3, 4, 5, 6, 7, 8, 9},
                {1, 2},
                {2, 8,-8},
                {0},
            }));
        CHECK( std::fgetc(file) == EOF );
    }

    SECTION( "Empty rows" ) {
        char str_file[] =
            "\n"
            "1,2\n"
            "2,8,-8\n"
            "\n"
            "0\n"
            "\n"
            ;
        std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

        CHECK( util::parse_csv(file) ==
            (std::vector<std::vector<int>>{
                {},
                {1, 2},
                {2, 8,-8},
                {},
                {0},
                {},
            }));
        CHECK( std::fgetc(file) == EOF );
    }

    SECTION( "Empty file" ) {
        char str_file[] =
            "\n"
            ;
        std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

        CHECK( util::parse_csv(file) ==
            (std::vector<std::vector<int>>{ {},
            }));
        CHECK( std::fgetc(file) == EOF );
    }
}
