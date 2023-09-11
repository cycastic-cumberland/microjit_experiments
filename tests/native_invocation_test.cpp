//
// Created by cycastic on 9/5/23.
//

#include <gtest/gtest.h>
#include <microjit/orchestrator.h>

class NativeInvocationTestFixture : public ::testing::Test {
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
    static int native_invoke(int, int b){
        return b;
    }
    static microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int, int>> create_simple_invoke(microjit::Ref<MicroJITOrchestrator> p_orchestrator){
        auto instance = p_orchestrator->create_instance<int, int, int>();

        auto main_scope = instance->get_function()->get_main_scope();
        auto var1 = main_scope->create_variable<int>();
        auto var2 = main_scope->create_variable<int>();
        main_scope->construct_from_argument(var1, 0);
        main_scope->construct_from_argument(var2, 1);
        auto call_vec = microjit::ArgumentsVector::create(var1->value_reference(), var2->value_reference());
        main_scope->invoke_native(native_invoke, call_vec, var1);
        main_scope->function_return(var1);

        return instance.unwrap();
    }
    static microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int, int>> create_nested_invoke(microjit::Ref<MicroJITOrchestrator> p_orchestrator){
        auto instance1 = p_orchestrator->create_instance<int, int, int>();
        auto main_scope1 = instance1->get_function()->get_main_scope();

        auto var1 = main_scope1->create_variable<int>();
        auto var2 = main_scope1->create_variable<int>();
        main_scope1->construct_from_argument(var1, 0);
        main_scope1->construct_from_argument(var2, 1);
        auto call_vec1 = microjit::ArgumentsVector::create(var1->value_reference(), var2->value_reference());
        main_scope1->invoke_native(native_invoke, call_vec1, var1);
        main_scope1->function_return(var1);

        auto instance2 = p_orchestrator->create_instance<int, int, int>();
        auto main_scope2 = instance2->get_function()->get_main_scope();

        auto var3 = main_scope2->create_variable<int>();
        auto var4 = main_scope2->create_variable<int>();
        main_scope2->construct_from_argument(var3, 0);
        main_scope2->construct_from_argument(var4, 1);
        auto call_vec2 = microjit::ArgumentsVector::create(var3->value_reference(), var4->value_reference());
        main_scope2->invoke_jit(instance1->get_function(), call_vec2, var3);
        main_scope2->function_return(var3);

        return instance2.unwrap();
    }
};

TEST_F(NativeInvocationTestFixture, SimpleInvocationOrderTest){
    int a = 19, b = 27;

    auto instance = create_simple_invoke(orchestrator);
    auto re = instance->call(a, b);
    ASSERT_EQ(re, b);
}

TEST_F(NativeInvocationTestFixture, NestedInvocationOrderTest){
    int a = 19, b = 27;

    auto instance = create_nested_invoke(orchestrator);
    auto re = instance->call(a, b);
    ASSERT_EQ(re, b);
}
