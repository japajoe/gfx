#ifndef GFX_TIME_HPP
#define GFX_TIME_HPP

#include <chrono>

namespace GFX
{
    typedef std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> Timer;

    struct TimerInfo
    {
        Timer tp1;
        Timer tp2;
        float deltaTime;
        TimerInfo()
        {
            tp1 = std::chrono::system_clock::now();
            tp1 = std::chrono::system_clock::now();
            deltaTime = 0;
        }

        void Update()
        {
            tp2 = std::chrono::system_clock::now();
            std::chrono::duration<float> elapsed = tp2 - tp1;
            tp1 = tp2;
            deltaTime = elapsed.count();
        }
    };

    class Time
    {
	friend class Application;
    private:
        static TimerInfo timer;
        static double time;
        static int FPS;
        static float elapsedTime;
        static float averageFPS;
		static void NewFrame();
    public:
        static float GetDeltaTime();
        static float GetFixedDeltaTime();
        static float GetFPS();
        static float GetTime();
        static double GetTimeAsDouble();
    };
};

#endif