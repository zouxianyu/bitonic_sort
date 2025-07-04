#include "benchmark/benchmark.h"
#include "plain_bitonic_sorter.h"
#include "std_thread_bitonic_sorter.h"
#include "openmp_bitonic_sorter.h"
#include "simd_bitonic_sorter.h"
#include <vector>
#include <algorithm> // For std::generate, std::iota
#include <random>    // For std::mt19937
#include <thread>    // For std::thread::hardware_concurrency

// Helper to generate data
static std::vector<int> generate_data(size_t size, const std::string& type = "random") {
    std::vector<int> data(size);
    if (type == "random") {
        std::mt19937 gen(42); // Fixed seed for reproducibility in data generation
        std::uniform_int_distribution<> distrib(0, size * 10);
        std::generate(data.begin(), data.end(), [&]() { return distrib(gen); });
    } else if (type == "sorted") {
        std::iota(data.begin(), data.end(), 0);
    } else if (type == "reversed") {
        std::iota(data.begin(), data.end(), 0);
        std::reverse(data.begin(), data.end());
    }
    // Add other types if needed: nearly_sorted, few_unique etc.
    return data;
}

// --- Plain Sorter Benchmark ---
static void BM_PlainBitonicSort(benchmark::State& state) {
    PlainBitonicSorter sorter;
    std::vector<int> data = generate_data(state.range(0));
    for (auto _ : state) {
        std::vector<int> current_data = data; // Copy data for each iteration
        sorter.sort(current_data, SortOrder::Ascending);
        benchmark::ClobberMemory(); // Prevent compiler from optimizing away the sort
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_PlainBitonicSort)->RangeMultiplier(2)->Range(1<<6, 1<<16)->Complexity(benchmark::oNLogN); // 64 to 65536

// --- Std::Thread Sorter Benchmark ---
static void BM_StdThreadBitonicSort(benchmark::State& state) {
    unsigned int num_threads = state.range(1);
    if (num_threads == 0) { // 0 could mean hardware_concurrency
        num_threads = std::thread::hardware_concurrency();
        if (num_threads == 0) num_threads = 2; // Default if detection fails
    }
    StdThreadBitonicSorter sorter(num_threads);
    std::vector<int> data = generate_data(state.range(0));

    for (auto _ : state) {
        std::vector<int> current_data = data;
        sorter.sort(current_data, SortOrder::Ascending);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
    state.counters["threads"] = num_threads;
}
// Test with a few thread counts, including 0 for dynamic detection
BENCHMARK(BM_StdThreadBitonicSort)
    ->ArgsProduct({
        benchmark::CreateRange(1<<6, 1<<16, 2), // Data sizes
        benchmark::CreateDenseRange(0, std::thread::hardware_concurrency() > 0 ? std::thread::hardware_concurrency() : 4, std::thread::hardware_concurrency() > 2 ? 2 : 1) // Thread counts (0, 2, 4, ...)
    })
    ->Complexity(benchmark::oNLogN);


// --- OpenMP Sorter Benchmark ---
static void BM_OpenMPBitonicSort(benchmark::State& state) {
    // OpenMP typically uses OMP_NUM_THREADS environment variable.
    // We can pass a thread count to the constructor if designed that way, or rely on env.
    // Assuming OpenMPBitonicSorter uses omp_set_num_threads or respects OMP_NUM_THREADS
    // For this benchmark, we'll just note that OpenMP controls threads externally or via its constructor if available.
    // The OpenMPBitonicSorter might need modification to accept thread counts, or we test default behavior.
    // For now, assuming default behavior. If it has a constructor for threads:
    // OpenMPBitonicSorter sorter(state.range(1)); // if it takes thread count
    OpenMPBitonicSorter sorter; // Assuming it uses max threads or OMP_NUM_THREADS
    std::vector<int> data = generate_data(state.range(0));

    for (auto _ : state) {
        std::vector<int> current_data = data;
        sorter.sort(current_data, SortOrder::Ascending);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
    // state.counters["threads"] = omp_get_max_threads(); // If inside parallel region or after setting
}
BENCHMARK(BM_OpenMPBitonicSort)->RangeMultiplier(2)->Range(1<<6, 1<<16)->Complexity(benchmark::oNLogN);

// --- SIMD Sorter Benchmark ---
static void BM_SIMDBitonicSort(benchmark::State& state) {
    SIMDBitonicSorter sorter;
    std::vector<int> data = generate_data(state.range(0));
    for (auto _ : state) {
        std::vector<int> current_data = data;
        sorter.sort(current_data, SortOrder::Ascending);
        benchmark::ClobberMemory();
    }
    state.SetComplexityN(state.range(0));
}
BENCHMARK(BM_SIMDBitonicSort)->RangeMultiplier(2)->Range(1<<6, 1<<16)->Complexity(benchmark::oNLogN);


BENCHMARK_MAIN();
