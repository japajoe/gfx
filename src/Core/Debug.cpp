#include "Debug.hpp"
#include <map>
#include <cstdlib> // for system()

namespace GFX
{
    DebugLogCallback Debug::callback = nullptr;
    
    static std::map<ConsoleColor, std::string> consoleColorMap = {
        { ConsoleColor::Red, "\x1B[31m" },
        { ConsoleColor::Green, "\x1B[32m" },
        { ConsoleColor::Yellow, "\x1B[33m" },
        { ConsoleColor::Blue, "\x1B[34m" },
        { ConsoleColor::Magenta, "\x1B[35m" },
        { ConsoleColor::Cyan, "\x1B[36m" },
        { ConsoleColor::White, "\x1B[37m" },
        { ConsoleColor::Reset, "\x1B[0m" }
    };

    void Debug::WriteLine(const std::string &text, ConsoleColor color)
    {
    #ifdef _WIN32
        std::cout << text << '\n';
    #else
        std::cout << consoleColorMap[color] << text << consoleColorMap[ConsoleColor::Reset] << '\n';
    #endif
    }

    void Debug::Write(const std::string &text, ConsoleColor color)
    {
    #ifdef _WIN32
        std::cout << text;
    #else
        std::cout << consoleColorMap[color] << text << consoleColorMap[ConsoleColor::Reset];
    #endif
    }

    void Debug::WriteLog(const std::string &text)
    {
    #ifdef GFX_DEBUG_LOGGING
    #ifdef _WIN32
        std::cout << text << '\n';
    #else
        std::cout << text << '\n';
    #endif

        if(callback)
            callback(text, ConsoleMessageType::Log);
    #endif
    }

    void Debug::WriteError(const std::string &text)
    {
    #ifdef GFX_DEBUG_LOGGING
    #ifdef _WIN32
        std::cout << text << '\n';
    #else
        std::cout << text << consoleColorMap[ConsoleColor::Reset] << '\n';
    #endif

        if(callback)
            callback(text, ConsoleMessageType::Error);
    #endif
    }

    void Debug::SetCallback(const DebugLogCallback &callback)
    {
        Debug::callback = callback;
    }
}