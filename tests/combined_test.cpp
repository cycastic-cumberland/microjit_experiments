//
// Created by cycastic on 8/29/23.
//

#include <gtest/gtest.h>
#include <microjit/orchestrator.h>
#include "../general_functionalities.h"

class CombinedTestFixture : public ::testing::Test {
protected:
    microjit::Ref<MicroJITOrchestrator> orchestrator{};
public:
    void SetUp() override {
        orchestrator = microjit::orchestrator(microjit::CompilationAgentSettings{microjit::CompilationAgentHandlerType::SINGLE_UNSAFE,
                                                                                           6, 1024 * 4, 8});
    }
    void TearDown() override {
        orchestrator = nullptr;
    }
};

TEST_F(CombinedTestFixture, RecursiveFibonacciTest) {
    static constexpr auto ceil = 15;
    auto instance = create_recursive_fibonacci(orchestrator);
    for (int i = 0; i < ceil; i++){
        auto native = recursive_fibonacci(i);
        auto jit = instance->call(i);
        EXPECT_EQ(native, jit);
    }
}

TEST_F(CombinedTestFixture, DynamicProgrammingFibonacciTest) {
    static constexpr auto ceil = 15;
    auto instance = create_dp_fibonacci(orchestrator);
    for (int i = 0; i < ceil; i++){
        auto native = dp_fibonacci(i);
        auto jit = instance->call(i);
        EXPECT_EQ(native, jit);
    }
}

TEST_F(CombinedTestFixture, GreatestCommonDivisorTest) {
    static constexpr auto ceil = 8;
    auto instance = create_gcd(orchestrator);
    for (int i = 1; i < ceil; i++){
        for (int j = 1; j < ceil; j++){
            auto native = gcd(i, j);
            auto jit = instance->call(i, j);
            EXPECT_EQ(native, jit);
        }
    }
}
