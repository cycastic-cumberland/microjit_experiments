//
// Created by cycastic on 8/17/23.
//

#include <gtest/gtest.h>
#include "general_experiments.h"

int TestStruct::count = 0;

int main(int argc, char** argv){
    fibonacci_test();
    return 0;
//    ::testing::InitGoogleTest(&argc, argv);
//    auto result = RUN_ALL_TESTS();
//    return result;
}
