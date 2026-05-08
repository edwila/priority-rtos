#pragma once

#include <mutex>
#include "rtos.hpp"

struct task;

class rtos;

namespace RTOS {
    struct mutex {
        std::mutex _m;
        
        int8_t acquire(task* acq_task, rtos* os);
        void free();

        task* current_task = nullptr;

        bool is_free() const;
    };
};