#include "Debug.hpp"
#include "../Graphics/Renderers/LineRenderer.hpp"
#include <map>
#include <cstdlib> // for system()

namespace GFX
{
    DebugLogCallback Debug::callback = nullptr;

    static std::map<ConsoleColor, std::string> consoleColorMap = {
        {ConsoleColor::Red, "\x1B[31m"},
        {ConsoleColor::Green, "\x1B[32m"},
        {ConsoleColor::Yellow, "\x1B[33m"},
        {ConsoleColor::Blue, "\x1B[34m"},
        {ConsoleColor::Magenta, "\x1B[35m"},
        {ConsoleColor::Cyan, "\x1B[36m"},
        {ConsoleColor::White, "\x1B[37m"},
        {ConsoleColor::Reset, "\x1B[0m"}};

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

        if (callback)
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

        if (callback)
            callback(text, ConsoleMessageType::Error);
#endif
    }

    void Debug::SetCallback(const DebugLogCallback &callback)
    {
        Debug::callback = callback;
    }

    void Debug::DrawLine(const Vector3 &p1, const Vector3 &p2, const Color &color)
    {
        LineRenderer::DrawLine(p1, p2, color);
    }

    void Debug::DrawSphere(const Vector3 &center, float radius, const Color &color, int segments, int latitudeCount)
    {
        // Ensure minimum divisions
        if (segments < 3)
            segments = 3;
        if (latitudeCount < 2)
            latitudeCount = 2;

        // --- Draw latitude circles (horizontal slices, excluding poles) ---
        // latitude indices from 1 to latitudeCount-2,
        // where index 0 is the south pole and latitudeCount-1 is the north pole.
        for (int lat = 1; lat < latitudeCount - 1; ++lat)
        {
            // Calculate the angle from the equator.
            // Theta varies from -PI/2 to PI/2.
            float theta = -M_PI / 2.0f + (M_PI * lat) / (latitudeCount - 1);
            // The radius of the circle at this latitude.
            float circleRadius = radius * cosf(theta);
            // The y-coordinate (vertical) offset.
            float y = radius * sinf(theta);

            Vector3 previousPoint;
            // Draw the circle at this latitude.
            for (int seg = 0; seg <= segments; ++seg)
            {
                float phi = (2.0f * M_PI * seg) / segments;
                float x = circleRadius * cosf(phi);
                float z = circleRadius * sinf(phi);

                Vector3 currentPoint = center + Vector3(x, y, z);
                if (seg > 0)
                {
                    Debug::DrawLine(previousPoint, currentPoint, color);
                }
                previousPoint = currentPoint;
            }
        }

        // --- Draw meridians (vertical slices from south to north poles) ---
        // Render 'segments' number of meridians.
        for (int seg = 0; seg < segments; ++seg)
        {
            float phi = (2.0f * M_PI * seg) / segments;
            Vector3 previousPoint;
            // Iterate over latitude from south pole (-PI/2) to north pole (PI/2)
            int steps = latitudeCount;
            for (int lat = 0; lat < steps; ++lat)
            {
                // Linear interpolation for theta from -PI/2 to PI/2.
                float theta = -M_PI / 2.0f + (M_PI * lat) / (steps - 1);
                float x = radius * cosf(theta) * cosf(phi);
                float y = radius * sinf(theta);
                float z = radius * cosf(theta) * sinf(phi);

                Vector3 currentPoint = center + Vector3(x, y, z);
                if (lat > 0)
                {
                    Debug::DrawLine(previousPoint, currentPoint, color);
                }
                previousPoint = currentPoint;
            }
        }
    }
}