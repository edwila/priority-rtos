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

            task* n_t;

            {
                std::lock_guard<std::mutex> l(q.m); // lock this priority's queue's mutex
                n_t = q.q.front();
                q.q.pop_front();

                if(q.q.empty()) bitmask.fetch_and(~(1 << next_task_set));
            } // Release the mutex here
            if(n_t != nullptr){
                bool completed = n_t->job();
                if(!completed){
                    // queue it again
                    schedule(n_t);
                } else{
                    delete n_t;
                }
            }
        }
    });

    init_thread.detach();
};

void rtos::boost_task(task* a, task* b){
    U8 a_prio = a->priority;
    a->priority = b->priority;
            
    {
        std::lock_guard<std::mutex> l(a->self_q->m);
        std::lock_guard<std::mutex> l2(b->self_q->m);

        a->self_q->q.pop_front(); // Remove from old priority queue
        if(a->self_q->q.empty()){
            bitmask.fetch_and(~(1 << a_prio)); // Remove it
        }
        a->self_q = b->self_q; // Set our new priority queue
        b->self_q->q.push_front(a); // Add to the new priority queue
    }
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

int8_t rtos::schedule(task* t){
    // schedule the task, <t>, t.priority
    task_queue& q = t_q[t->priority];

    if(t->self_q == nullptr){
        t->self_q = &q;
    }

    {
        std::lock_guard<std::mutex> l(q.m);
        
        if(q.q.size() >= MAX_QUEUE_SIZE) return -1;

        q.q.push_back(t);
    }

    bitmask.fetch_or(1 << t->priority);

    return 1;
};