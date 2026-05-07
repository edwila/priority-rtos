#include "parser.hpp"

bool parser::parse(const std::string& raw_cmd, std::chrono::steady_clock::time_point& start_time){
    size_t idx = raw_cmd.find(';');
    if(idx == std::string::npos) return false;
    size_t cmd_idx = 0;
    while(idx != std::string::npos){
        size_t space = raw_cmd.find(' ', cmd_idx);

        if(space == std::string::npos) continue;

        std::string cmd = raw_cmd.substr(cmd_idx, space-cmd_idx);

        command* cmd_used = nullptr;

        size_t next_end = raw_cmd.find(';', space+1);
        if(next_end == std::string::npos) continue;
        std::string cmd_str = raw_cmd.substr(space+1, next_end-space-1);

        if(cmd == "print"){
            cmd_used = new print(start_time, cmd_str);
        } else if(cmd == "sleep"){
            cmd_used = new sleep_cmd(start_time, static_cast<uint32_t>(std::stoi(cmd_str)));
        }

        if(cmd_used != nullptr){
            commands.push_back(cmd_used);
        }

        cmd_idx = idx+2; // Accounts for the space after ';'
        idx = raw_cmd.find(';', idx+1);
    }

    return true;
};

bool parser::step(){
    // Once a command is parsed (via parser:parse()), step through the execution. Return true if it's not a sleep and if we completed this command
    if(current_command >= commands.size()) return true;

    if(commands[current_command]->execute() > 0){
        current_command++; // We finished running this command, move to the next one
    }

    return false;
};