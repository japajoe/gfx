#ifndef GFX_CONCURRENTLIST_HPP
#define GFX_CONCURRENTLIST_HPP

#include <vector>
#include <algorithm>
#include <mutex>
#include <stdexcept>

namespace GFX
{
    template<typename T>
    class ConcurrentList
    {
    private:
        std::vector<T> items;
        mutable std::mutex mtx;
    public:
        T &operator[](size_t index)
        {
            std::lock_guard<std::mutex> lock(mtx);
            if (index >= items.size())
                throw std::out_of_range("Index out of range");
            return items[index];
        }

        void Add(const T &item)
        {
            std::lock_guard<std::mutex> lock(mtx);
            items.push_back(item);
        }

        void Remove(const T &item)
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = std::find(items.begin(), items.end(), item);
            if (it != items.end())
            {
                items.erase(it);
            }
        }

        size_t GetCount() const
        {
            std::lock_guard<std::mutex> lock(mtx);
            return items.size();
        }
    };
}

#endif