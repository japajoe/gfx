#ifndef GFX_OBJECT_HPP
#define GFX_OBJECT_HPP

#include <cstdint>
#include <string>

namespace GFX
{
    class Object
    {
    private:
        uint64_t id;
        std::string name;
    public:
        Object();
        virtual ~Object();
        uint64_t GetInstanceId() const;
        void SetName(const std::string name);
        std::string GetName() const;
    };
}

#endif