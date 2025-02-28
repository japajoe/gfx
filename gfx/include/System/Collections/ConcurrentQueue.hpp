#ifndef GFX_CONCURRENTQUEUE_HPP
#define GFX_CONCURRENTQUEUE_HPP

#include <queue>
#include <mutex>
#include <condition_variable>

namespace GFX
{
    template <typename T>
    class ConcurrentQueue
    {
    private:
        std::queue<T> items;
        mutable std::mutex mutex;
        std::condition_variable condition;
    public:
        void Enqueue(const T &item)
        {
            std::lock_guard<std::mutex> lock(mutex);
            items.push(item);
            condition.notify_one();
        }

        bool TryDequeue(T &item)
        {
            std::lock_guard<std::mutex> lock(mutex);

            if (items.empty())
                return false;

            item = items.front();
            items.pop();
            return true;
        }

        void Clear()
        {
            std::lock_guard<std::mutex> lock(mutex);
            while (!items.empty()) 
            {
                items.pop();
            }
        }

        std::size_t GetCount() const
        {
            std::lock_guard<std::mutex> lock(mutex);
            return items.size();
        }
    };
}

#endif