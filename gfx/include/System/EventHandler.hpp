#ifndef GFX_EVENTHANDLER_HPP
#define GFX_EVENTHANDLER_HPP

#include <vector>
#include <functional>

namespace GFX
{
    template<typename T>
    class EventHandler
    {
    public:
        std::vector<T> callbacks;

        template<typename ... Param>
        void operator () (Param ... param)
        {
            for(size_t i = 0; i < callbacks.size(); i++)
            {
                if(callbacks[i])
                {
                    callbacks[i](param...);
                }
            }
        }

        void operator += (T callback)
        {
            callbacks.push_back(callback);
        }
    };
}

#endif