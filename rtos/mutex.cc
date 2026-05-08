#include "mutex.hpp"
#include <iostream>

int8_t RTOS::mutex::acquire(task* acq_task, rtos* os){
    if(is_free()){
        _m.lock();
        current_task = acq_task;
        return 1;
    } else{
        // Check if current_task->priority < acq_task->priority
        // If it is, then we want to boost current_task's priority to acq_task temporarily just to let it finish running
        if(current_task->priority > acq_task->priority){
            os->boost_task(current_task, acq_task);
        }
    }

    return 0;
}

void RTOS::mutex::free(){
    _m.unlock();
    current_task = nullptr;
}

bool RTOS::mutex::is_free() const {
    return current_task == nullptr;
}
