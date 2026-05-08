#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <string>
#include <chrono>
#include "../rtos/mutex.hpp"
#include "../rtos/rtos.hpp"

inline std::function<std::string()> cli_prompt_hook = []() { return ">> "; };

inline RTOS::mutex add_mutex;

template<typename... Args>
inline void out(Args&&... args){
    std::cout << "\r\033[2K";
    ((std::cout << std::forward<Args>(args)), ...);
    std::cout << "\n" << cli_prompt_hook() << std::flush;
};

struct command {
    // Parent struct
    std::chrono::steady_clock::time_point start; // Timestamp for when the rtos has ran. Populated via ctor
    task* t;
    rtos* cur_os;
    command(std::chrono::steady_clock::time_point& start_time, task* T, rtos* os);
    virtual int8_t execute() = 0;
    virtual ~command() = default;
};

struct print : public command {
    std::string content;
    print(std::chrono::steady_clock::time_point& start_time, std::string& s, task* t, rtos* os);
    int8_t execute();
};

struct sleep_cmd : public command {
    bool started = false;
    std::chrono::steady_clock::time_point command_end;
    uint32_t dur;

    sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t sleep_dur, task* t, rtos* os);
    int8_t execute();
};

struct add_cmd : public command {
    // Add 2 numbers, then print the result
    uint32_t x, y;
    add_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t a, uint32_t b, task* t, rtos* os);
    int8_t execute();
};

struct add_sleep_cmd : public command {
    // Add 2 numbers, sleep for 5s, then print the result
    bool started = false;
    std::chrono::steady_clock::time_point command_end;
    uint32_t dur = 5000;
    uint32_t x, y;
    add_sleep_cmd(std::chrono::steady_clock::time_point& start_time, uint32_t a, uint32_t b, task* t, rtos* os);
    int8_t execute();
};