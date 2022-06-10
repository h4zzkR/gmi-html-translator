#pragma once

#include <array>
#include <filesystem>

namespace fs = std::filesystem;

/*
 * Блок задач копирования фиксированного размера.
 */
struct CopyBatch {
    static const size_t kNumBuckets = 12;

    bool Add(const fs::directory_entry &file) {
        if (ptr == kNumBuckets) {
            return false;
        }
        buckets[ptr++] = file;
        return true;
    }

    CopyBatch() = default;

    CopyBatch(CopyBatch &&cp) : ptr(cp.ptr), buckets(std::move(cp.buckets)) {
        cp.ptr = 0;
    }

    std::array<fs::directory_entry, kNumBuckets> buckets;
    size_t ptr = 0;
};