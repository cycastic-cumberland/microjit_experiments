//
// Created by cycastic on 8/30/23.
//

#ifndef MICROJIT_EXPERIMENT_COMBINED_TEST_BUILDER_H
#define MICROJIT_EXPERIMENT_COMBINED_TEST_BUILDER_H

#include <microjit/orchestrator.h>

static microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int>> create_recursive_fibonacci(microjit::Ref<MicroJITOrchestrator> p_orchestrator){
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

static microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int>> create_dp_fibonacci(microjit::Ref<MicroJITOrchestrator> p_orchestrator) {
    auto instance = p_orchestrator->create_instance<int, int>();

    auto main_scope = instance->get_function()->get_main_scope();
    auto num = main_scope->create_variable<int>();
    main_scope->construct_from_argument(num, 0);
    auto imm0 = microjit::ImmediateValue::create<int>(0);
    auto is_zero = main_scope->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_EQUAL,
                    num->value_reference(), imm0);
    auto is_zero_branch = main_scope->if_branch(is_zero);
    is_zero_branch->function_return(num);

    auto prev = main_scope->create_variable<int>();
    auto curr = main_scope->create_variable<int>();
    auto i = main_scope->create_variable<int>();
    main_scope->construct_from_immediate(prev, 0);
    main_scope->construct_from_immediate(curr, 1);
    main_scope->construct_from_immediate(i, 2);
    auto imm1 = microjit::ImmediateValue::create<int>(1);
    auto while_cond = main_scope->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_LESSER_OR_EQUAL,
                    i->value_reference(), num->value_reference());
    auto loop = main_scope->while_branch(while_cond);
    auto incr_expr = loop->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_ADD,
                    i->value_reference(), imm1);
    auto new_curr = loop->create_variable<int>();
    auto raise_curr_expr = loop->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_ADD,
                    curr->value_reference(), prev->value_reference());
    loop->construct_from_primitive_atomic_expression(new_curr, raise_curr_expr);
    loop->assign_from_variable<int>(prev, curr);
    loop->assign_from_variable<int>(curr, new_curr);
    loop->assign_from_primitive_atomic_expression(i, incr_expr);
    main_scope->function_return(curr);
    return instance.unwrap();
}

static microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int, int>> create_gcd(microjit::Ref<MicroJITOrchestrator> p_orchestrator) {
    auto instance = p_orchestrator->create_instance<int, int, int>();

    auto main_scope = instance->get_function()->get_main_scope();
    auto a = main_scope->create_variable<int>();
    auto b = main_scope->create_variable<int>();
    main_scope->construct_from_argument(a, 0);
    main_scope->construct_from_argument(b, 1);

    auto imm0 = microjit::ImmediateValue::create(0);

    auto loop_cond = main_scope->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_NOT_EQUAL,
                    b->value_reference(), imm0);

    auto loop = main_scope->while_branch(loop_cond);

    auto temp = loop->create_variable<int>();
    loop->construct_from_variable(temp, b);

    auto reassign_b = loop->primitive_binary_expression_parser()
            ->parse(microjit::AbstractOperation::BINARY_MOD,
                    a->value_reference(), b->value_reference());

    loop->assign_from_primitive_atomic_expression(b, reassign_b);
    loop->assign_from_variable<int>(a, temp);

    main_scope->function_return(a);

    return instance.unwrap();
}

static int recursive_fibonacci(int num){
    if (num < 2) return num;
    return recursive_fibonacci(num - 1) + recursive_fibonacci(num - 2);
}
static int dp_fibonacci(int num){
    if (num == 0) return 0;
    int prev = 0;
    int curr = 1;
    for (int i = 2; i <= num; ++i) {
        int new_curr = curr + prev;
        prev = curr;
        curr = new_curr;
    }
    return curr;
}
static int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

#endif //MICROJIT_EXPERIMENT_COMBINED_TEST_BUILDER_H
