#include "test_evutils.h"
#include "test_lulog.hpp"
#include "test_lu_erron.hpp"
#include "test_lu_hash_table.hpp"
#include "test_lummpool.hpp"



int main(int argc, char **argv) {
   ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();

}

