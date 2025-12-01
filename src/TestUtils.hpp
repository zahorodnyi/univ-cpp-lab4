#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include "ConcurrentStructure.hpp"

enum class OpType { READ, WRITE, STRING_OP };

struct Instruction {
    OpType type;
    int index;
    int value;
};

void generateTestFiles(const std::string& baseName, int opCount, const std::vector<double>& weights, int numFiles) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(weights.begin(), weights.end());

    for (int f = 0; f < numFiles; ++f) {
        std::ofstream file(baseName + "_" + std::to_string(f) + ".txt");
        for (int i = 0; i < opCount; ++i) {
            int op = d(gen);
            switch (op) {
                case 0: file << "read 0\n"; break;
                case 1: file << "write 0 1\n"; break;
                case 2: file << "read 1\n"; break;
                case 3: file << "write 1 1\n"; break;
                case 4: file << "read 2\n"; break;
                case 5: file << "write 2 1\n"; break;
                case 6: file << "string\n"; break;
            }
        }
    }
}

std::vector<Instruction> loadInstructions(const std::string& filename) {
    std::vector<Instruction> instructions;
    std::ifstream file(filename);
    std::string line, type;
    int idx, val;

    if (!file.is_open()) return instructions;

    while (file >> type) {
        Instruction instr{};
        if (type == "read") {
            file >> idx;
            instr.type = OpType::READ;
            instr.index = idx;
        } else if (type == "write") {
            file >> idx >> val;
            instr.type = OpType::WRITE;
            instr.index = idx;
            instr.value = val;
        } else if (type == "string") {
            instr.type = OpType::STRING_OP;
        }
        instructions.push_back(instr);
    }
    return instructions;
}

void worker(ConcurrentStructure& structure, const std::vector<Instruction>& ops) {
    for (const auto& op : ops) {
        switch (op.type) {
            case OpType::READ: {
                volatile int val = structure.read(op.index);
                (void)val;
                break;
            }
            case OpType::WRITE: {
                structure.write(op.index, op.value);
                break;
            }
            case OpType::STRING_OP: {
                std::string s = structure;
                volatile size_t len = s.length();
                (void)len;
                break;
            }
        }
    }
}

void runBenchmark(const std::string& scenarioName, const std::string& baseFilename, int numThreads) {
    ConcurrentStructure ds;

    std::vector<std::vector<Instruction>> allThreadOps;
    for (int i = 0; i < numThreads; ++i) {
        std::string fname = baseFilename + "_" + std::to_string(i) + ".txt";
        allThreadOps.push_back(loadInstructions(fname));
    }

    auto start = std::chrono::high_resolution_clock::now();

    std::vector<std::thread> threads;
    threads.reserve(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(worker, std::ref(ds), std::ref(allThreadOps[i]));
    }

    for (auto& t : threads) {
        t.join();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Scenario: " << scenarioName
              << " | Threads: " << numThreads
              << " | Time: " << duration.count() << " ms" << std::endl;
}