#pragma once

#include <atomic>
#include <array>
#include "log_entry.h"

static constexpr size_t CACHE_LINE_SIZE = 64;

template<size_t Size>
class LockFreeQueue {
    static_assert((Size & (Size - 1)) == 0, "Size must be power of 2");
    static constexpr size_t MASK = Size - 1;

private:
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> head_{0};
    alignas(CACHE_LINE_SIZE) std::atomic<uint64_t> tail_{0};
    alignas(CACHE_LINE_SIZE) std::array<LogEntry, Size> buffer_;

public:
    LockFreeQueue() = default;
    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

    bool try_push(const LogEntry& entry) {
        const uint64_t current_head = head_.load(std::memory_order_relaxed);
        const uint64_t next_head = current_head + 1;
        
        if (next_head == tail_.load(std::memory_order_acquire)) [[unlikely]] {
            return false;
        }
        
        buffer_[current_head & MASK] = entry;
        head_.store(next_head, std::memory_order_release);
        return true;
    }

    bool try_pop(LogEntry& entry) {
        const uint64_t current_tail = tail_.load(std::memory_order_relaxed);
        
        if (current_tail == head_.load(std::memory_order_acquire)) [[likely]] {
            return false;
        }
        
        entry = buffer_[current_tail & MASK];
        tail_.store(current_tail + 1, std::memory_order_release);
        return true;
    }

    size_t size() const {
        const uint64_t h = head_.load(std::memory_order_acquire);
        const uint64_t t = tail_.load(std::memory_order_acquire);
        return h - t;
    }

    bool empty() const {
        return head_.load(std::memory_order_acquire) == 
               tail_.load(std::memory_order_acquire);
    }

    static constexpr size_t capacity() {
        return Size - 1;
    }

    bool is_nearly_full(double threshold = 0.8) const {
        return size() >= static_cast<size_t>(capacity() * threshold);
    }
};

using DefaultLogQueue = LockFreeQueue<1024>;
using LargeLogQueue = LockFreeQueue<4096>;
using SmallLogQueue = LockFreeQueue<256>;