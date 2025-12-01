#include <iostream>
#include <vector>
#include <filesystem>
#include "TestUtils.hpp"

int main() {

    std::filesystem::remove("tests/test_a_0.txt");
    std::filesystem::remove("tests/test_a_1.txt");
    std::filesystem::remove("tests/test_a_2.txt");
    std::filesystem::remove("tests/test_b_0.txt");
    std::filesystem::remove("tests/test_b_1.txt");
    std::filesystem::remove("tests/test_b_2.txt");
    std::filesystem::remove("tests/test_c_0.txt");
    std::filesystem::remove("tests/test_c_1.txt");
    std::filesystem::remove("tests/test_c_2.txt");

    const int OP_COUNT = 100000;
    const std::vector<int> THREAD_COUNTS = {1, 2, 3};

    std::vector<double> weightsA = {10, 5, 10, 5, 10, 20, 40};
    generateTestFiles("tests/test_a", OP_COUNT, weightsA, 3);

    std::vector<double> weightsB(7, 1.0);
    generateTestFiles("tests/test_b", OP_COUNT, weightsB, 3);

    std::vector<double> weightsC = {0, 0, 0, 0, 0, 10, 90};
    generateTestFiles("tests/test_c", OP_COUNT, weightsC, 3);

    std::cout << "-----------------------------------" << std::endl;
    std::cout << "Starting Benchmarks" << std::endl;
    std::cout << "-----------------------------------" << std::endl;

    for (int t : THREAD_COUNTS) {
        runBenchmark("A (Variant 11)", "tests/test_a", t);
    }
    std::cout << "-----------------------------------" << std::endl;

    for (int t : THREAD_COUNTS) {
        runBenchmark("B (Equal)", "tests/test_b", t);
    }
    std::cout << "-----------------------------------" << std::endl;

    for (int t : THREAD_COUNTS) {
        runBenchmark("C (Worst Case)", "tests/test_c", t);
    }
    std::cout << "-----------------------------------" << std::endl;

    return 0;
}