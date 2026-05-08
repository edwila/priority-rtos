#include "commands.hpp"

// Command stuff
command::command(std::chrono::steady_clock::time_point& start_time, task* T, rtos* os){
    start = start_time;
    t = T;
    cur_os = os;
};



// Print stuff
print::print(std::chrono::steady_clock::time_point& start_time, std::string& s, task* t, rtos* os) : command(start_time, t, os){
    content = s;
};

int8_t print::execute(){
    out(content);
    return 1; // 1 = error-free completion
};



// Sleep stuff
sleep_cmd::sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t sleep_dur, task* t, rtos* os) : command(start_time, t, os), dur(sleep_dur){};

int8_t sleep_cmd::execute(){
    if(!started){
        command_end = std::chrono::steady_clock::now() + std::chrono::milliseconds(dur);
        started = true;
        return 0; // No error + no successful completion, get's attempted by RTOS scheduler next cycle
    } else{
        if(std::chrono::steady_clock::now() >= command_end){
            return 1; // We completed our sleep!
        } else{
            return 0;
        }
    }
};



// Add stuff
add_cmd::add_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t a, uint32_t b, task* t, rtos* os) : command(start_time, t, os), x(a), y(b) {};

int8_t add_cmd::execute(){
    int8_t i = add_mutex.acquire(t, cur_os);
    if(i == 0){
        return 0;
    }
    out((x+y));
    add_mutex.free();
    return 1;
};



// Add_sleep stuff
add_sleep_cmd::add_sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t a, uint32_t b, task* t, rtos* os) : command(start_time, t, os), x(a), y(b) {};

int8_t add_sleep_cmd::execute(){
    if(!started){
        int8_t i = add_mutex.acquire(t, cur_os);
        if(i == 0) return 0;
        command_end = std::chrono::steady_clock::now() + std::chrono::milliseconds(dur);
        started = true;
        return 0; // No error + no successful completion, get's attempted by RTOS scheduler next cycle
    } else{
        if(std::chrono::steady_clock::now() >= command_end){
            out((x+y));
            add_mutex.free();
            return 1; // We completed our sleep!
        } else{
            return 0;
        }
    }
};