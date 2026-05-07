#include "../rtos/rtos.hpp"
#include "../Parser/parser.hpp"
#include <memory>

int main(){
    std::string cmd = "";

    rtos os;

    cli_prompt_hook = [](){
        return "[RTOS Terminal] ";
    };

    out("Launched RTOS.");

    while(cmd != "exit"){
        std::cin >> cmd;

        if(cmd == "schedule"){
            int prio_input;
            std::cin >> prio_input >> std::ws; 
            U8 priority = static_cast<U8>(prio_input);

            std::getline(std::cin, cmd);

            auto par = std::make_shared<parser>();
            auto tm = os.get_time();

            par->parse(cmd, tm);

            os.schedule(task{
                [cmd, par, tm]() -> bool {
                    return par->step();
                },
                priority
            });

            out("Successfully scheduled a new job with priority [", prio_input, "].");
        }
    }

    return 1;
}