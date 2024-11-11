#pragma once    

#include <string>

struct Log {
    std::string message;
    bool forTerminal;

    // Optional: Constructor for convenience
    Log(const std::string& msg, bool terminalOutput)
        : message(msg), forTerminal(terminalOutput) {}
};