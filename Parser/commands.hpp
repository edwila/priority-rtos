#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>

inline std::function<std::string()> cli_prompt_hook = []() { return ">> "; };

template<typename... Args>
inline void out(Args&&... args){
    std::cout << "\r\033[2K";
    ((std::cout << std::forward<Args>(args)), ...);
    std::cout << "\n" << cli_prompt_hook() << std::flush;
}

struct command {
    // Parent struct
    std::chrono::steady_clock::time_point start; // Timestamp for when the rtos has ran. Populated via ctor
    command(std::chrono::steady_clock::time_point& start_time);
    virtual int8_t execute() = 0;
    virtual ~command() = default;
};

struct print : public command {
    std::string content;
    print(std::chrono::steady_clock::time_point& start_time, std::string& s);
    int8_t execute();
};

struct sleep_cmd : public command {
    bool started = false;
    std::chrono::steady_clock::time_point command_end;
    uint32_t dur;

    sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t sleep_dur);
    int8_t execute();
};