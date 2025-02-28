#include "Object.hpp"
#include <utility>

namespace GFX
{
    Object::Object()
    {
        this->id = reinterpret_cast<uint64_t>(this);
    }

    Object::~Object()
    {

    }

    uint64_t Object::GetInstanceId() const
    {
        return id;
    }

    void Object::SetName(const std::string name)
    {
        this->name = name;
    }

    std::string Object::GetName() const
    {
        return name;
    }
}