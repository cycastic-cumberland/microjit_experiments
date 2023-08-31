//
// Created by cycastic on 8/31/23.
//

#include <benchmark/benchmark.h>
#include <microjit/orchestrator.h>
#include "../general_functionalities.h"

static thread_local auto orchestrator = microjit::orchestrator();
static thread_local microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int>> jit_recursive_fib
    = create_recursive_fibonacci(orchestrator);
static thread_local microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int>> jit_dp_fib
    = create_dp_fibonacci(orchestrator);
static thread_local microjit::Ref<MicroJITOrchestrator::FunctionInstance<int, int, int>> jit_gcd
    = create_gcd(orchestrator);

static void BM_NativeRecursiveFibonacci(benchmark::State& state) {
    int n = int(state.range(0));
    for (auto _ : state) {
        benchmark::DoNotOptimize(recursive_fibonacci(n));
    }
}

static void BM_JitRecursiveFibonacci(benchmark::State& state){
//    if (jit_recursive_fib == nullptr)
//        jit_recursive_fib = create_recursive_fibonacci(orchestrator);
    int n = int(state.range(0));
    for (auto _ : state) {
        benchmark::DoNotOptimize(jit_recursive_fib->call(n));
    }
}

static void BM_NativeDPFibonacci(benchmark::State& state) {
    int n = int(state.range(0));
    for (auto _ : state) {
        benchmark::DoNotOptimize(dp_fibonacci(n));
    }
}

static void BM_JitDPFibonacci(benchmark::State& state){
//    if (jit_dp_fib == nullptr)
//        jit_dp_fib = create_dp_fibonacci(orchestrator);
    int n = int(state.range(0));
    for (auto _ : state) {
        benchmark::DoNotOptimize(jit_dp_fib->call(n));
    }
}

static void BM_NativeGCD(benchmark::State& state) {
    int a = int(state.range(0));
    int b = int(state.range(1));
    for (auto _ : state) {
        benchmark::DoNotOptimize(gcd(a, b));
    }
}

static void BM_JitGCD(benchmark::State& state){
//    if (jit_gcd == nullptr)
//        jit_gcd = create_gcd(orchestrator);
    int a = int(state.range(0));
    int b = int(state.range(1));
    for (auto _ : state) {
        benchmark::DoNotOptimize(jit_gcd->call(a, b));
    }
}

// Anything pass 20 and it runs slow af
BENCHMARK(BM_NativeRecursiveFibonacci)->Range(1, 10);
BENCHMARK(BM_JitRecursiveFibonacci)->Range(1, 10);

BENCHMARK(BM_NativeDPFibonacci)->Range(1, 100);
BENCHMARK(BM_JitDPFibonacci)->Range(1, 100);

BENCHMARK(BM_NativeGCD)->RangePair(1, 1000, 100, 500);
BENCHMARK(BM_JitGCD)->RangePair(1, 1000, 100, 500);
