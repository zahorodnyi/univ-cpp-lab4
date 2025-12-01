#pragma once

#include <string>
#include <mutex>
#include <sstream>

#ifdef __APPLE__
    constexpr size_t CACHE_LINE = 128;
#else
    constexpr size_t CACHE_LINE = 64;
#endif

struct alignas(CACHE_LINE) Field {
    int value;
    std::mutex mtx;

    Field() : value(0) {}
};

class ConcurrentStructure {
private:
    static const int SIZE = 3;
    Field fields[SIZE];

public:
    int read(int index) {
        if (index < 0 || index >= SIZE) return -1;
        std::lock_guard<std::mutex> lock(fields[index].mtx);
        return fields[index].value;
    }

    void write(int index, int value) {
        if (index < 0 || index >= SIZE) return;
        std::lock_guard<std::mutex> lock(fields[index].mtx);
        fields[index].value = value;
    }

    operator std::string() {
        std::stringstream ss;
        std::scoped_lock lock(fields[0].mtx, fields[1].mtx, fields[2].mtx);
        for (int i = 0; i < SIZE; ++i) {
            ss << fields[i].value << " ";
        }
        return ss.str();
    }
};