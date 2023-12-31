//
// Created by cycastic on 8/15/23.
//

#ifndef MICROJIT_TEST_H
#define MICROJIT_TEST_H

#include <chrono>
#include <iostream>

#include <microjit/instructions.h>
#include <microjit/jit_x86_64.h>
#include <microjit/orchestrator.h>

#include "general_functionalities.h"
#include "microjit/trampoline.h"


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

    TestStruct() : a(), b() {
        count++;
    }
    TestStruct(const float& p_a, const double& p_b) : a(p_a), b(p_b) {
        count++;
    }
    TestStruct(const TestStruct& p_other) : a(p_other.a), b(p_other.b){
        count++;
    }
    TestStruct& operator=(const TestStruct& p_other){
        a = p_other.a;
        b = p_other.b;
        return *this;
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
        main_scope->default_construct<TestStruct>(var1);
//        main_scope->construct_from_argument(var1, 0);
        auto scope = main_scope->create_scope();
        auto var2 = scope->create_variable<TestStruct>();
        scope->construct_from_argument(var2, 0);
        scope->assign_from_variable<TestStruct>(var1, var2);
        main_scope->function_return(var1);
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

void test_jit_call(){
    using namespace microjit;
    static const std::function<int(int)> model1{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto func1 = orchestrator->create_instance_from_model(model1);
    auto func2 = orchestrator->create_instance_from_model(model1);
    auto f2_main_scope = func2->get_function()->get_main_scope();
    auto f2_var3 = f2_main_scope->create_variable<long>();
    auto f2_var1 = f2_main_scope->create_variable<int>();
    auto f2_var2 = f2_main_scope->create_variable<long>();
    f2_main_scope->construct_from_argument(f2_var1, 0);
    f2_main_scope->function_return(f2_var1);
    auto f1_main_scope = func1->get_function()->get_main_scope();
    auto f1_var3 = f1_main_scope->create_variable<long>();
    auto f1_var1 = f1_main_scope->create_variable<int>();
    auto f1_var2 = f1_main_scope->create_variable<long>();
    auto args_vec = Ref<ArgumentsVector>::make_ref();
    std::vector<Ref<Value>> vec_values = { ArgumentValue::create(0).c_style_cast<Value>() };
    args_vec->values.swap(vec_values);
    f1_main_scope->invoke_jit(func2->get_function(), args_vec,
                              f1_var1);
    f1_main_scope->function_return(f1_var1);
    auto re = func1(141232);
    std::cout << re << "\n";
}

void test_jit_call2(){
    using namespace microjit;
    std::cout << "Current count: " << TestStruct::count << "\n";
    {
        static const std::function<TestStruct(TestStruct)> model1{};
        auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
        auto func1 = orchestrator->create_instance_from_model(model1);
        auto func2 = orchestrator->create_instance_from_model(model1);
        auto f2_main_scope = func2->get_function()->get_main_scope();
        auto f2_var1 = f2_main_scope->create_variable<TestStruct>();
        auto f2_var2 = f2_main_scope->create_variable<TestStruct>();
        f2_main_scope->default_construct<TestStruct>(f2_var2);
        f2_main_scope->construct_from_argument(f2_var1, 0);
        f2_main_scope->function_return(f2_var1);
        auto f1_main_scope = func1->get_function()->get_main_scope();
        auto f1_var1 = f1_main_scope->create_variable<TestStruct>();
        auto f1_var2 = f1_main_scope->create_variable<TestStruct>();
        f1_main_scope->default_construct<TestStruct>(f1_var2);
        auto args_vec = Ref<ArgumentsVector>::make_ref();
        std::vector<Ref<Value>> vec_values = { ArgumentValue::create(0).c_style_cast<Value>() };
        args_vec->values.swap(vec_values);
        f1_main_scope->invoke_jit(func2->get_function(), args_vec,
                                  f1_var1);
        f1_main_scope->function_return(f1_var1);
        std::cout << "Before invocation: " << TestStruct::count << "\n";
        auto re = func1(TestStruct(12, 13));
        std::cout << re.a << " " << re.b << "\n";
        std::cout << "After invocation: " << TestStruct::count << "\n";
//        std::cout << re.a << " " << re.b << "\n";
    }
    std::cout << "After full clean up: " << TestStruct::count << "\n";
}
template <typename F, typename ... Args>
static _ALWAYS_INLINE_ uint64_t time_function(F&& f, Args&&...args)
{
    auto start_time = std::chrono::high_resolution_clock::now();
    std::forward<F>(f)(std::forward<Args>(args)...);
    auto finish_time = std::chrono::high_resolution_clock::now();
    auto duration = finish_time - start_time;
    auto epoch_us = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count();
    return static_cast<uint64_t>(epoch_us);
}

float simple_float_square(float a){
    auto re = a * a;
    return re;
}

void test_primitive_expression(){
    using namespace microjit;
    typedef float ret_type;
    static const std::function<ret_type(ret_type)> model1{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance_from_model(model1);
    auto scope = instance->get_function()->get_main_scope();
    auto var1 = scope->create_variable<ret_type>();
    auto var2 = scope->create_variable<ret_type>();
    scope->construct_from_argument(var2, 0);
    auto expr_builder = scope->primitive_binary_expression_parser();
    auto expr = expr_builder->parse(AbstractOperation::BINARY_MUL,
                                               var2->value_reference(),
                                               var2->value_reference());
    scope->assign_from_primitive_atomic_expression(var1, expr);
    scope->function_return(var1);
    auto native = time_function(simple_float_square, 4.0);
    std::cout << "First native time: " << native << " ns\n";
    auto native_2nd = time_function(simple_float_square, 4.0);
    std::cout << "Second native time: " << native_2nd << " ns\n";
    auto lazy = time_function(instance, 4.0);
    std::cout << "First JIT (compile and) run time: " << lazy << " ns\n";
    auto compiled = time_function(instance, 4.0);
    std::cout << "Second JIT run time: " << compiled << " ns\n";
    auto compiled_again = time_function(instance, 4.0);
    std::cout << "Third JIT run time: " << compiled_again << " ns\n";
    auto compiled_again_and_again = time_function(instance, 4.0);
    std::cout << "Forth JIT run time: " << compiled_again_and_again << " ns\n";
}

microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int>> create_fibonacci(microjit::Ref<MicroJITOrchestrator>& p_orchestrator){
    auto instance = p_orchestrator->create_instance<int, int>();

    auto main_scope = instance->get_function()->get_main_scope();
    auto arg = main_scope->create_variable<int>();
    main_scope->construct_from_argument(arg, 0);
    auto imm1 = microjit::ImmediateValue::create<int>(1);
    auto imm2 = microjit::ImmediateValue::create<int>(2);
    auto imm_expr = main_scope->primitive_binary_expression_parser()
                                          ->parse(microjit::AbstractOperation::BINARY_LESSER_OR_EQUAL,
                                                  arg->value_reference(), imm1);
    auto first_check = main_scope->if_branch(imm_expr);
    first_check->function_return(arg);
    auto pass1 = main_scope->create_variable<int>();
    auto pass2 = main_scope->create_variable<int>();
    auto p1_expr = main_scope->primitive_binary_expression_parser()
                                         ->parse(microjit::AbstractOperation::BINARY_SUB,
                                              arg->value_reference(), imm1);
    auto p2_expr = main_scope->primitive_binary_expression_parser()
                                         ->parse(microjit::AbstractOperation::BINARY_SUB,
                                                 arg->value_reference(), imm2);
    auto re1 = main_scope->create_variable<int>();
    auto re2 = main_scope->create_variable<int>();
    main_scope->assign_from_primitive_atomic_expression(pass1, p1_expr);
    main_scope->assign_from_primitive_atomic_expression(pass2, p2_expr);
    auto call_vec1 = microjit::ArgumentsVector::create(pass1->value_reference());
    auto call_vec2 = microjit::ArgumentsVector::create(pass2->value_reference());
    main_scope->invoke_jit(instance->get_function(), call_vec1, re1);
    main_scope->invoke_jit(instance->get_function(), call_vec2, re2);
    auto composite = main_scope->primitive_binary_expression_parser()->parse(microjit::AbstractOperation::BINARY_ADD,
                                                                                        re1->value_reference(),
                                                                                        re2->value_reference());
    main_scope->assign_from_primitive_atomic_expression(re1, composite);
    main_scope->function_return(re1);
    return instance.unwrap();
}

void recursive_fibonacci_test(){
    using namespace microjit;
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = create_recursive_fibonacci(orchestrator);
    instance->recompile();
    microjit::OrchestratorComponent<MicroJITCompiler_x86_64>::InstanceWrapper<int, int> wrapped(instance);
    static constexpr auto num1 = 6;

    auto native1    = time_function(recursive_fibonacci, num1);
    auto native2    = time_function(recursive_fibonacci, num1);
    auto native3    = time_function(recursive_fibonacci, num1);
    auto jit1       = time_function(wrapped, num1);
    auto jit2       = time_function(wrapped, num1);
    auto jit3       = time_function(wrapped, num1);
    std::cout << "Native 1: " << native1 << " ns\n";
    std::cout << "Native 2: " << native2 << " ns\n";
    std::cout << "Native 3: " << native3 << " ns\n";

    std::cout << "JIT 1: " << jit1 << " ns | Percentile: " << (double(native1) / double(jit1) * 100.0) << "%\n";
    std::cout << "JIT 2: " << jit2 << " ns | Percentile: " << (double(native2) / double(jit2) * 100.0) << "%\n";
    std::cout << "JIT 3: " << jit3 << " ns | Percentile: " << (double(native3) / double(jit3) * 100.0) << "%\n";

    wrapped.detach();
}

void print_stuff(int a, int b){
//    fprintf(stdout, "%d %d\n", a, b);
    std::cout << a << " " << b << "\n";
}

void native_invocation_test(){
    using namespace microjit;
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance<void, int, int>();
    auto main_scope = instance->get_function()->get_main_scope();
    auto var1 = main_scope->create_variable<int>();
    auto var2 = main_scope->create_variable<int>();
    main_scope->construct_from_argument(var1, 0);
    main_scope->construct_from_argument(var2, 1);
    auto arg_vec = ArgumentsVector::create(VariableValue::create(var1), VariableValue::create(var2));
    main_scope->invoke_native(print_stuff, arg_vec);
    main_scope->function_return();

    auto instance2 = orchestrator->create_instance<void, int, int>();
    auto main_scope2 = instance2->get_function()->get_main_scope();
    auto var3 = main_scope2->create_variable<int>();
    auto var4 = main_scope2->create_variable<int>();
    main_scope2->construct_from_argument(var3, 0);
    main_scope2->construct_from_argument(var4, 1);
    auto arg_vec2 = ArgumentsVector::create(VariableValue::create(var3), VariableValue::create(var4));
    main_scope2->invoke_jit(instance->get_function(), arg_vec2);
    main_scope2->function_return();

    instance2(2, 3);

    volatile auto a = 0;
}

void dp_fibonacci_test(){
    using namespace microjit;
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = create_dp_fibonacci(orchestrator);
    instance->recompile();
    microjit::OrchestratorComponent<MicroJITCompiler_x86_64>::InstanceWrapper<int, int> wrapped(instance);
    static constexpr auto num1 = 6;

    auto native1    = time_function(dp_fibonacci, num1);
    auto native2    = time_function(dp_fibonacci, num1);
    auto native3    = time_function(dp_fibonacci, num1);
    auto jit1       = time_function(wrapped, num1);
    auto jit2       = time_function(wrapped, num1);
    auto jit3       = time_function(wrapped, num1);
    std::cout << "Native 1: " << native1 << " ns\n";
    std::cout << "Native 2: " << native2 << " ns\n";
    std::cout << "Native 3: " << native3 << " ns\n";

    std::cout << "JIT 1: " << jit1 << " ns | Percentile: " << (double(native1) / double(jit1) * 100.0) << "%\n";
    std::cout << "JIT 2: " << jit2 << " ns | Percentile: " << (double(native2) / double(jit2) * 100.0) << "%\n";
    std::cout << "JIT 3: " << jit3 << " ns | Percentile: " << (double(native3) / double(jit3) * 100.0) << "%\n";

    wrapped.detach();
}

void atomic_boolean_overhead_test(){
    SafeFlag flag{};
    bool a;
    for (int i = 0; i < 1000; i++)
        a = flag.is_set();
}

void boolean_overhead_test(){
    bool flag = true;
    bool a;
    for (int i = 0; i < 1000; i++)
        a = flag;
}

void boolean_read_test(){
    auto atomic = time_function(atomic_boolean_overhead_test);
    auto non_atomic = time_function(boolean_overhead_test);
    std::cout << "Atomic: " << atomic << "ns\n";
    std::cout << "Non-atomic: " << non_atomic << "ns\n";
    std::cout << "Overhead: " << (double(atomic) / double(non_atomic) * 100.0) << "%\n";
}

void print_1(int a){
    std::cout << a << "\n";
}
void print_2(int a, int b){
    std::cout << a << " " << b << "\n";
}
int fetch(int a, int b){
    return a - b;
}



void test_branching(){
    using namespace microjit;
    typedef int ret_type;
    static const std::function<ret_type(ret_type)> model1{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance = orchestrator->create_instance_from_model(model1);
    auto scope = instance->get_function()->get_main_scope();
//    auto var1 = scope->create_variable<ret_type>();
    auto var2 = scope->create_variable<int>();
    scope->assign_from_immediate(var2, 0);
    auto imm = ImmediateValue::create(12);
    auto imm2 = ImmediateValue::create(5);
    auto imm3 = ImmediateValue::create(9);
    auto expr = scope->primitive_binary_expression_parser()->parse(AbstractOperation::BINARY_LESSER,
                                   var2->value_reference(), imm);
    auto loop = scope->while_branch(expr);
    auto expr2 = loop->primitive_binary_expression_parser()->parse(AbstractOperation::BINARY_ADD,
                                     var2->value_reference(), imm2);
    auto expr3 = loop->primitive_binary_expression_parser()->parse(AbstractOperation::BINARY_GREATER,
                                     var2->value_reference(), imm3);
    auto break_scope = loop->if_branch(expr3);
    break_scope->break_loop();
    loop->assign_from_primitive_atomic_expression(var2, expr2);
    scope->function_return(var2);
    auto time1 = time_function(instance, 12);
    auto time2 = time_function(instance, 1422);
    auto time3 = time_function(instance, 12123);
    std::cout << time1 << " ns\n";
    std::cout << time2 << " ns\n";
    std::cout << time3 << " ns\n";
    auto re = instance(12);
    std::cout << re;
}


void test_new_op(){
    using namespace microjit;
    typedef int ret_type;
    static const std::function<ret_type(ret_type, ret_type)> model1{};
    auto orchestrator = Ref<MicroJITOrchestrator>::make_ref();
    auto instance1 = orchestrator->create_instance_from_model(model1);
    auto instance2 = orchestrator->create_instance_from_model(model1);
    auto scope1 = instance1->get_function()->get_main_scope();
    auto scope2 = instance2->get_function()->get_main_scope();
    auto var2_1 = scope2->create_variable<ret_type>();
    auto var2_2 = scope2->create_variable<ret_type>();
    scope2->construct_from_argument(var2_1, 0);
    scope2->construct_from_argument(var2_2, 1);
    auto expr = scope2->primitive_binary_expression_parser()->parse(AbstractOperation::BINARY_SUB,
                                                                    var2_1->value_reference(),
                                                                    var2_2->value_reference());
    scope2->assign_from_primitive_atomic_expression(var2_1, expr);
    scope2->function_return(var2_1);
    auto var1_1 = scope1->create_variable<ret_type>();
    auto var1_2 = scope1->create_variable<ret_type>();
    scope1->construct_from_argument(var1_1, 0);
    scope1->construct_from_argument(var1_2, 1);
    auto args_vec = ArgumentsVector::create(var1_1->value_reference(), var1_2->value_reference());
    scope1->invoke_jit(instance2->get_function(), args_vec, var1_1);
    scope1->function_return(var1_1);
    auto ret = instance1(12, 13);
    std::cout << ret;
}

#endif //MICROJIT_TEST_H
