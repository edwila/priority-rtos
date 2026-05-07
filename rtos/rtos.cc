#include "rtos.hpp"

rtos::rtos(){
    active.store(true);
    idling.store(true);

    start_time = std::chrono::steady_clock::now();

    init_thread = std::thread([this](){
        while(this->active.load()){
            // Compare to 0. If 0, idle. Otherwise, complete the next highest priority task (calculate via __builtin_ctz())
            // This 32-bit bitmask essentially tells us if there are scheduled jobs that need to be completed
            while(bitmask.load() == 0){
                idle_task.job();
            }

            U32 bm = bitmask.load();

            // We got an actual task now
            U8 next_task_set = __builtin_ctz(bm);
            task_queue& q = t_q[next_task_set];

            task n_t;

            {
                std::lock_guard<std::mutex> l(q.m); // lock this priority's queue's mutex
                n_t = std::move(q.q.front());
                q.q.pop();

                if(q.q.empty()) bitmask.fetch_and(~(1 << next_task_set));
            } // Release the mutex here
            bool completed = n_t.job();
            if(!completed){
                // queue it again
                schedule(std::move(n_t));
            }
        }
    });

    init_thread.detach();
};

bool rtos::is_active() const {
    return active;
};

bool rtos::is_idle() const {
    return idling;
};

std::chrono::steady_clock::time_point rtos::get_time() const {
    return start_time;
};

int8_t rtos::schedule(task&& t){
    // schedule the task, <t>, t.priority
    task_queue& q = t_q[t.priority];

    {
        std::lock_guard<std::mutex> l(q.m);
        
        if(q.q.size() >= MAX_QUEUE_SIZE) return -1;

        q.q.push(std::move(t));
    }

    bitmask.fetch_or(1 << t.priority);

    return 1;
};