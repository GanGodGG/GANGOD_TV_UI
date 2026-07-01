#include <gangod/actiondictionary.h>

void Log(gargs args){
    for(auto& arg : args){
        std::cout << arg << " ";
    }
    std::cout << "\n";
}

void GANGOD_REGISTER_NAME::setAllRegisters(){
    GActions::REGISTER(Log, "Log");
}