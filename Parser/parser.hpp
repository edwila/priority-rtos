#include "commands.hpp"
#include <vector>
#include <thread>

struct parser {
    std::vector<command*> commands;
    uint16_t current_command = 0; // Keeps track of our current command so we can defer execution if there is a sleep

    bool parse(const std::string& raw_cmd, std::chrono::steady_clock::time_point& start_time); // Parses raw_cmd and populates this->commands, then returns true on success or false on failure.
    bool step(); // Runs commands stored in this->commands. Returns true on completion, false on incompletion.
};