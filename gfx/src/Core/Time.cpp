#include "Time.hpp"
#include "../Physics/Physics.hpp"

namespace GFX
{
    TimerInfo Time::timer;
    double Time::time = 0.0;
    int Time::FPS = 0;
    float Time::elapsedTime = 0.0f;
    float Time::averageFPS = 0.0f;

    void Time::NewFrame()
    {
        timer.Update();
        
        time += timer.deltaTime;

        elapsedTime += timer.deltaTime;
        FPS++;

        if (elapsedTime > 0.5f)
        {
            averageFPS = FPS / elapsedTime;
            FPS = 0;
            elapsedTime = 0.0f;
        }
    }

    float Time::GetDeltaTime()
    {
        return timer.deltaTime;
    }

    float Time::GetFixedDeltaTime()
    {
        return Physics::GetFixedTimeStep();
    }

    float Time::GetFPS()
    {
        return averageFPS;
    }

    float Time::GetTime()
    {
        return static_cast<float>(time);
    }

    double Time::GetTimeAsDouble()
    {
        return time;
    }
}