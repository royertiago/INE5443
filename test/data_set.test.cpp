#include "pr/data_set.h"
#include <catch.hpp>

TEST_CASE( "Trivial DataSet member test", "[DataSet][trivial]" ) {
    DataSet dataset( std::vector<std::string>{ "X pos", "Y pos" },
                     std::vector<std::string>{ "Color" },
                     std::vector<DataEntry>{
                         DataEntry({1, 1},{"Blue"}),
                         DataEntry({2, 4},{"Red"}),
                         DataEntry({4, 0},{"Green"})
                     }
                    );

    REQUIRE( dataset.size() == 3 );
    REQUIRE( dataset.attribute_count() == 2 );
    REQUIRE( dataset.category_count() == 1 );

    CHECK( dataset.attribute_name(0) == "X pos" );
    CHECK( dataset.attribute_name(1) == "Y pos" );
    CHECK( dataset.category_name(0) == "Color" );

    auto it = dataset.begin();
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({1, 1},{"Blue"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({2, 4},{"Red"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({4, 0},{"Green"}) );

    ++it;
    CHECK( it == dataset.end() );
}

char str_file[] =
    "# Commentary that should be ignored.\n"
    "# Another commentary.\n"
    "n 3\n"
    "a X pos\n"
    "a Y pos\n"
    "c Color\n"
    "\n"
    "1,1,Blue\n"
    "2,4,Red\n"
    "4,0,Green\n"
    ;

TEST_CASE( "DataSet parsing from file", "[DataSet][parse]" ) {
    std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");

    DataSet dataset = DataSet::parse(file);

    REQUIRE( dataset.size() == 3 );
    REQUIRE( dataset.attribute_count() == 2 );
    REQUIRE( dataset.category_count() == 1 );

    CHECK( dataset.attribute_name(0) == "X pos" );
    CHECK( dataset.attribute_name(1) == "Y pos" );
    CHECK( dataset.category_name(0) == "Color" );

    auto it = dataset.begin();
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({1, 1},{"Blue"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({2, 4},{"Red"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({4, 0},{"Green"}) );

    ++it;
    CHECK( it == dataset.end() );
}

TEST_CASE( "DataSet parsing - corner cases", "[DataSet][parse]" ) {
    DataSet dataset;
    std::FILE * file;

    char consecutive_commentaries[] = "#\n#\n#\nn 1\na X\n\n2\n";
    file = fmemopen(consecutive_commentaries, sizeof(consecutive_commentaries)-1, "r");

    REQUIRE_NOTHROW( dataset = DataSet::parse(file) );
    CHECK( dataset.size() == 1 );
    CHECK( dataset.attribute_count() == 1 );
    CHECK( dataset.category_count() == 0 );
    CHECK( dataset.attribute_name(0) == "X" );
    CHECK( *dataset.begin() == DataEntry({2},{}) );

    std::fclose(file);

    char no_entries[] = "n 1\na X\n\n";
    file = fmemopen(no_entries, sizeof(no_entries)-1, "r");

    REQUIRE_NOTHROW( dataset = DataSet::parse(file) );
    CHECK( dataset.size() == 0 );
    CHECK( dataset.attribute_count() == 1 );
    CHECK( dataset.category_count() == 0 );
    CHECK( dataset.attribute_name(0) == "X" );

    std::fclose(file);

    char both_cases[] = "#\n#\n#\nn 1\na X\n\n";
    file = fmemopen(both_cases, sizeof(both_cases)-1, "r");

    REQUIRE_NOTHROW( dataset = DataSet::parse(file) );
    CHECK( dataset.size() == 0 );
    CHECK( dataset.attribute_count() == 1 );
    CHECK( dataset.category_count() == 0 );
    CHECK( dataset.attribute_name(0) == "X" );

    std::fclose(file);
}

TEST_CASE( "DataSet writing to file", "[DataSet][write]" ) {
    std::FILE * file = fmemopen(str_file, sizeof(str_file)-1, "r");
    DataSet dataset = DataSet::parse( file );
    std::fclose( file );
    char output_file[2*sizeof(str_file)];

    file = fmemopen(output_file, 2*sizeof(str_file)-1, "w");
    dataset.write( file );
    std::fclose( file );
    CHECK( output_file == std::string(
        "n 3\n"
        "a X pos\n"
        "a Y pos\n"
        "c Color\n"
        "\n"
        "1.000000,1.000000,Blue\n"
        "2.000000,4.000000,Red\n"
        "4.000000,0.000000,Green\n"
        )
    );

    file = fmemopen(output_file, 2*sizeof(str_file)-1, "w");
    dataset.write( file, "aca" );
    std::fclose( file );
    CHECK( output_file == std::string(
        "n 3\n"
        "a X pos\n"
        "c Color\n"
        "a Y pos\n"
        "\n"
        "1.000000,Blue,1.000000\n"
        "2.000000,Red,4.000000\n"
        "4.000000,Green,0.000000\n"
        )
    );

    file = fmemopen(output_file, 2*sizeof(str_file)-1, "w");
    dataset.write( file, "caa" );
    std::fclose( file );
    CHECK( output_file == std::string(
        "n 3\n"
        "c Color\n"
        "a X pos\n"
        "a Y pos\n"
        "\n"
        "Blue,1.000000,1.000000\n"
        "Red,2.000000,4.000000\n"
        "Green,4.000000,0.000000\n"
        )
    );

    file = fmemopen(output_file, 2*sizeof(str_file)-1, "w");
    dataset.write( file, "c" );
    std::fclose( file );
    CHECK( output_file == std::string(
        "n 3\n"
        "c Color\n"
        "a X pos\n"
        "a Y pos\n"
        "\n"
        "Blue,1.000000,1.000000\n"
        "Red,2.000000,4.000000\n"
        "Green,4.000000,0.000000\n"
        )
    );
}
