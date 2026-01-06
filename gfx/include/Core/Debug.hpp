#ifndef GFX_DEBUG_HPP
#define GFX_DEBUG_HPP

#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Quaternion.hpp"
#include "../Graphics/Color.hpp"
#include <string>
#include <functional>
#include <iostream>
#include <sstream>

//#define GRAVY_DEBUG_LOGGING

namespace GFX
{
    enum class ConsoleColor
    {
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Reset
    };

    enum class ConsoleMessageType
    {
        Log,
        Error
    };

    using DebugLogCallback = std::function<void(const std::string &message, ConsoleMessageType type)>;

    class Debug
    {
    private:
        static DebugLogCallback callback;
    public:
        static void WriteLine(const std::string &text, ConsoleColor color = ConsoleColor::White);
        static void Write(const std::string &text, ConsoleColor color = ConsoleColor::White);
        static void WriteLog(const std::string &text);
        static void WriteError(const std::string &text);
        static void SetCallback(const DebugLogCallback &callback);
        static void DrawLine(const Vector3 &p1, const Vector3 &p2, const Color &color);
        static void DrawSphere(const Vector3 &center, float radius, const Quaternion &rotation, const Color &color, int segments = 36, int latitudeCount = 8);
        static void DrawCube(const Vector3 &center, const Vector3 &size, const Quaternion &rotation, const Color &color);
        static void DrawBounds(const Vector3 &min, const Vector3 &max, const Vector3 &position, const Quaternion &rotation, const Color &color);

        template<typename... Args>
        static void Write(const std::string &format, Args... args) 
        {
            std::string buffer;
            if(FormatArgs(format, buffer, args...))
            {
                Write(buffer);
            }
        }

        template<typename... Args>
        static void WriteLine(const std::string &format, Args... args) 
        {
            std::string buffer;
            if(FormatArgs(format, buffer, args...))
            {
                WriteLine(buffer);
            }
        }
        
        template<typename... Args>
        static void WriteLog(const std::string &format, Args... args) 
        {
            std::string buffer;
            if(FormatArgs(format, buffer, args...))
            {
                WriteLog(buffer);
            }
        }

        template<typename... Args>
        static void WriteError(const std::string &format, Args... args) 
        {
            std::string buffer;
            if(FormatArgs(format, buffer, args...))
            {
                WriteError(buffer);
            }
        }

        template<typename... Args>
        static bool FormatArgs(const std::string &format, std::string &buffer, Args... args) 
        {
            int size = std::snprintf(nullptr, 0, format.c_str(), args...);
            if (size < 0) 
            {
                std::cerr << "Error: snprintf failed!" << std::endl;
                return false;
            }

            buffer.resize(size + 1);
            buffer[buffer.size() - 1] = '\0';
            std::snprintf(&buffer[0], buffer.size(), format.c_str(), args...);
            return true;
        }
    };
}

#endif