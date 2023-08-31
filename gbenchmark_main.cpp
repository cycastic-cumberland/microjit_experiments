//
// Created by cycastic on 8/31/23.
//

#include <benchmark/benchmark.h>

int main(int argc, char** argv){
    ::benchmark::Initialize(&argc, argv);
    ::benchmark::RunSpecifiedBenchmarks();
    return 0;
}
