#include "../rtos/rtos.hpp"
#include "../Parser/parser.hpp"
#include <memory>

int main(){
    std::string cmd = "";

    rtos os;

    cli_prompt_hook = [](){
        return "[RTOS Terminal] ";
    };

    while(cmd != "exit"){
        out("");
        std::cin >> cmd;

        if(cmd == "schedule"){
            int prio_input;
            std::cin >> prio_input >> std::ws; 
            U8 priority = static_cast<U8>(prio_input);

            std::getline(std::cin, cmd);

            auto par = std::make_shared<parser>();
            auto tm = os.get_time();

            task* t = new task{
                [par]() -> bool {
                    return par->step();
                },
                priority,
                nullptr
            };

            par->parse(cmd, tm, t, &os);

            os.schedule(t);
        }
    }

    return 1;
}