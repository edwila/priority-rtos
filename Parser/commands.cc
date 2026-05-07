#include "commands.hpp"

// Command stuff
command::command(std::chrono::steady_clock::time_point& start_time){
    start = start_time;
};



// Print stuff
print::print(std::chrono::steady_clock::time_point& start_time, std::string& s) : command(start_time){
    content = s;
};

int8_t print::execute(){
    out(content);
    return 1; // 1 = error-free completion
};



// Sleep stuff
sleep_cmd::sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t sleep_dur) : command(start_time), dur(sleep_dur){};

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