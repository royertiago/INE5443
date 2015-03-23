/* Utility program.
 * Rewrites the dataset to put attributes before categories.
 * Input: stdin
 * Output: stdout */
#include "pr/data_set.h"

int main() {
    DataSet dataset = DataSet::parse( stdin );
    dataset.write( stdout );
    return 0;
}
