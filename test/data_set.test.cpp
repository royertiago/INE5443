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

    CHECK( dataset.mean() == DataEntry({7.0/3, 5.0/3},{}) );

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

TEST_CASE( "DataSet appending", "[DataSet][push_back][trivial]" ) {
    DataSet dataset( 2, 1 );
    CHECK( dataset.size() == 0 );
    CHECK( dataset.attribute_count() == 2 );
    CHECK( dataset.category_count() == 1 );

    dataset.push_back( DataEntry({2,4},{"A"}) );
    REQUIRE( dataset.size() == 1 );
    CHECK( *dataset.begin() == DataEntry({2,4},{"A"}) );
    CHECK( dataset.mean() == DataEntry({2,4},{}) );

    dataset.push_back( DataEntry({9,0},{"B"}) );
    CHECK( dataset.size() == 2 );
    CHECK( dataset.mean() == DataEntry({5.5,2},{}) );

    auto it = dataset.begin();
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({2,4},{"A"}) );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({9,0},{"B"}) );

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

char str_nfile[] =
    "# Commentary that should be ignored.\n"
    "# Another commentary.\n"
    "n 4\n"
    "a X pos\n"
    "i Name\n"
    "a Y pos\n"
    "c Color\n"
    "\n"
    "1,Alpha,1,Blue\n"
    "2,Beta,4,Red\n"
    "4,Gamma,0,Green\n"
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
    CHECK( dataset.mean() == DataEntry({7.0/3, 5.0/3},{}) );

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

TEST_CASE( "DataSet parsing from file with names", "[DataSet][parse]" ) {
    std::FILE * file = fmemopen(str_nfile, sizeof(str_nfile)-1, "r");

    DataSet dataset = DataSet::parse(file);

    REQUIRE( dataset.size() == 3 );
    REQUIRE( dataset.attribute_count() == 2 );
    REQUIRE( dataset.category_count() == 1 );

    CHECK( dataset.attribute_name(0) == "X pos" );
    CHECK( dataset.attribute_name(1) == "Y pos" );
    CHECK( dataset.category_name(0) == "Color" );
    CHECK( dataset.mean() == DataEntry({7.0/3, 5.0/3},{}) );

    auto it = dataset.begin();
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({1, 1},{"Blue"}, "Alpha") );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({2, 4},{"Red"}, "Beta") );

    ++it;
    REQUIRE( it != dataset.end() );
    CHECK( *it == DataEntry({4, 0},{"Green"}, "Gamma") );

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
    CHECK( dataset.mean() == DataEntry({2},{}) );

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
TEST_CASE( "DataSet writing to file with names", "[DataSet][write]" ) {
    std::FILE * file = fmemopen(str_nfile, sizeof(str_nfile)-1, "r");
    DataSet dataset = DataSet::parse( file );
    std::fclose( file );
    char output_file[2*sizeof(str_nfile)];

    file = fmemopen(output_file, 2*sizeof(str_nfile)-1, "w");
    dataset.write( file );
    std::fclose( file );
    CHECK( output_file == std::string(
        "n 4\n"
        "a X pos\n"
        "a Y pos\n"
        "c Color\n"
        "i \n"
        "\n"
        "1.000000,1.000000,Blue,Alpha\n"
        "2.000000,4.000000,Red,Beta\n"
        "4.000000,0.000000,Green,Gamma\n"
        )
    );
}

TEST_CASE( "DataSet::category_satistics", "[DataSet]" ) {
    DataSet dataset(
        std::vector<std::string>(),
        std::vector<std::string>{ "Color", "Material" },
        std::vector<DataEntry>{
            DataEntry({},{"Blue", "Wood"}),
            DataEntry({},{"Red", "Steel"}),
            DataEntry({},{"Green", "Plastic"}),
            DataEntry({},{"Blue", "Plastic"}),
            DataEntry({},{"Yellow", "Wood"}),
            DataEntry({},{"Yellow", "Plastic"})
        }
    );
    std::vector<std::vector<std::pair<std::string, std::size_t>>> stat;
    REQUIRE_NOTHROW( stat = dataset.category_statistics() );
    REQUIRE( stat.size() == 2 );
    REQUIRE( stat[0].size() == 4 );
    REQUIRE( stat[1].size() == 3 );
    CHECK( stat[0][0].first == "Blue" );
    CHECK( stat[0][0].second == 2 );
    CHECK( stat[0][1].first == "Green" );
    CHECK( stat[0][1].second == 1 );
    CHECK( stat[0][2].first == "Red" );
    CHECK( stat[0][2].second == 1 );
    CHECK( stat[0][3].first == "Yellow" );
    CHECK( stat[0][3].second == 2 );
    CHECK( stat[1][0].first == "Plastic" );
    CHECK( stat[1][0].second == 3 );
    CHECK( stat[1][1].first == "Steel" );
    CHECK( stat[1][1].second == 1 );
    CHECK( stat[1][2].first == "Wood" );
    CHECK( stat[1][2].second == 2 );

    dataset.shuffle();
    CHECK( stat == dataset.category_statistics() );
}

TEST_CASE( "DataSet::standardize_factor", "[DataSet]" ) {
    DataSet dataset(
        std::vector<std::string>{"Value"},
        std::vector<std::string>(),
        std::vector<DataEntry>{
            DataEntry({2},{}),
            DataEntry({4},{}),
            DataEntry({4},{}),
            DataEntry({4},{}),
            DataEntry({5},{}),
            DataEntry({5},{}),
            DataEntry({7},{}),
            DataEntry({9},{}),
        }
    );
    auto pair = dataset.standardize_factor();
    CHECK( pair.first.size() == 1 );
    CHECK( pair.second.size() == 1 );
    CHECK( pair.first[0] == Approx(1/2.0) );
    CHECK( pair.second[0] == Approx(5.0) );

    dataset.standardize();
    auto it = dataset.begin();
    CHECK( it->attribute(0) == Approx(-1.5) );
    ++it;
    CHECK( it->attribute(0) == Approx(-0.5) );
    ++it;
    CHECK( it->attribute(0) == Approx(-0.5) );
    ++it;
    CHECK( it->attribute(0) == Approx(-0.5) );
    ++it;
    CHECK( it->attribute(0) == Approx(0.0) );
    ++it;
    CHECK( it->attribute(0) == Approx(0.0) );
    ++it;
    CHECK( it->attribute(0) == Approx(1.0) );
    ++it;
    CHECK( it->attribute(0) == Approx(2.0) );
    ++it;
    CHECK( it == dataset.end() );
}
