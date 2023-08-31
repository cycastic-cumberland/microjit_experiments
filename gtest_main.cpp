//
// Created by cycastic on 8/17/23.
//

#include <gtest/gtest.h>

int main(int argc, char** argv){
    ::testing::InitGoogleTest(&argc, argv);
    auto result = RUN_ALL_TESTS();
    return result;
}
