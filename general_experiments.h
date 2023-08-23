//
// Created by cycastic on 8/15/23.
//

#ifndef MICROJIT_TEST_H
#define MICROJIT_TEST_H

#include <microjit/instructions.h>
#include <microjit/jit_x86_64.h>
#include <microjit/orchestrator.h>
#include <microjit/decaying_weighted_cache.h>

#include <asmjit/asmjit.h>

#include <iostream>

void test_core_nodes_1() {
    using namespace microjit;
    static const std::function<void()> model{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance_from_model(model);
    instance();
}

struct TestStruct {
    float a;
    double b;

    static int count;

    TestStruct(const float& p_a, const double& p_b) : a(p_a), b(p_b) {
        count++;
    }
    TestStruct(const TestStruct& p_other) : a(p_other.a), b(p_other.b){
        count++;
    }
    ~TestStruct() {
        count--;
    }
};

void test_core_nodes_2() {
    using namespace microjit;
    static const std::function<int()> model{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance_from_model(model);
    auto function = instance->get_function();
    auto main_scope = function->get_main_scope();
    auto var1 = main_scope->create_variable<int>();
    auto var2 = main_scope->create_variable<int>();
    main_scope->construct_from_immediate(var1, 1320);
    main_scope->construct_from_argument(var1, 0);
    auto sub_scope = main_scope->create_scope();
    sub_scope->assign_from_immediate(var1, 11271277);
    sub_scope->assign_from_variable<int>(var2, var1);
//    main_scope->construct_from_immediate(var1, TestStruct(12, 15));
    sub_scope->function_return(var2);
    instance->recompile();
    auto re = instance();
    std::cout << re;
}

void test_core_nodes_3(){
    using namespace microjit;
    std::cout << "Current count: " << TestStruct::count << "\n";
    {
        static const std::function<TestStruct(TestStruct)> model{};
        auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
        auto instance = orchestrator->create_instance_from_model(model);
        auto function = instance->get_function();
        auto main_scope = function->get_main_scope();
        auto var1 = main_scope->create_variable<TestStruct>();
        main_scope->construct_from_argument(var1, 0);
        auto scope = main_scope->create_scope();
        auto var2 = scope->create_variable<TestStruct>();
        scope->construct_from_variable(var2, var1);
        scope->function_return(var2);
        instance->recompile();
        std::cout << "Before invocation: " << TestStruct::count << "\n";
        auto re = instance(TestStruct(12, 13));
        std::cout << "After invocation: " << TestStruct::count << "\n";
//        std::cout << re.a << " " << re.b << "\n";
    }
    std::cout << "After full clean up: " << TestStruct::count << "\n";
}

void test_core_nodes_4() {
    using namespace microjit;
    static const std::function<double()> model{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance_from_model(model);
    auto function = instance->get_function();
    auto main_scope = function->get_main_scope();
    auto var1 = main_scope->create_variable<double>();
    auto var2 = main_scope->create_variable<uint8_t>();
    main_scope->default_construct<double>(var1);
    main_scope->construct_from_immediate<uint8_t>(var2, 106);
    auto sub_scope = main_scope->create_scope();
    sub_scope->convert<uint8_t, double>(var2, var1);
    sub_scope->function_return(var1);
    instance->recompile();
    auto re = instance();
    std::cout << (re);
    instance->detach();
}

void test_weighted_cache(){
    using namespace microjit;
    DecayingWeightedCache<int, int> cache(2, 2);
    auto val1 = cache.push(1, 2);
    auto val2 = cache.push(2, 3);
    auto val3 = cache.push(3, 4);
    val1->heat = 15;
    val2->heat = 7;
    val3->heat = 35;
    cache.heap_sort();
    for (const auto& elem : cache.get_heap()) {
        std::cout << "(" << elem->key << ", " << elem->value << ", " << elem->heat << ")\n";
    }
    cache.manual_cleanup();
    for (const auto& elem : cache.get_heap()) {
        std::cout << "(" << elem->key << ", " << elem->value << ", " << elem->heat << ")\n";
    }
}

#endif //MICROJIT_TEST_H
