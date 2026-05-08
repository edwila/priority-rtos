#pragma once

#include <iostream> // std::cout
#include <functional> // std::function
#include <queue> // std::queue
#include <cstdint> // uint
#include <chrono> // time
#include <thread> // std::this_thread
#include <atomic> // std::atomic
#include "immintrin.h" // __builtin_ctz()
#include "mutex.hpp" // rtos::mutex

using U8 = uint8_t;
using U16 = uint16_t;
using U32 = uint32_t;
using U64 = uint64_t;

static constexpr U16 MAX_QUEUE_SIZE = 0xFFFF; // Maximum size of each priority's queue
static constexpr U8 MAX_PRIORITIES = 32; // 0-31 priorities, lower number = higher priority

struct task_queue;

struct task {
    std::function<bool()> job; // This will hold the callback that "task" is scheduled to complete. Returns true if completed, otherwise false
    U8 priority;
    task_queue* self_q;
};

struct task_queue {
    std::deque<task*> q;
    std::mutex m; // mutex for read/write processes on this queue
};

class rtos {
    public:
        int8_t schedule(task* t);
        void boost_task(task* a, task* b); // Boost a's priority to match b's priority
        bool is_active() const;
        bool is_idle() const;
        std::chrono::steady_clock::time_point get_time() const;
        rtos(); // Initialize variables, then also start the initialization thread
    private:
        task_queue t_q[MAX_PRIORITIES];
        std::atomic<bool> active; // true while the OS is running
        std::atomic<bool> idling; // true while the OS is idling
        
        std::atomic<U32> bitmask;

        std::thread init_thread;

        std::chrono::steady_clock::time_point start_time;

        static inline task idle_task = { []() -> bool { std::this_thread::yield(); return true; }, 31, nullptr }; // Default idle task. static inline so that it's shared and we can define it here
};