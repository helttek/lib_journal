#pragma once

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() = default;

    ThreadSafeQueue(const ThreadSafeQueue &) = delete;
    ThreadSafeQueue &operator=(const ThreadSafeQueue &) = delete;

    void push(T value)
    {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            queue_.push(std::move(value));
        }
        condvar_.notify_one();
    }

    void close()
    {
        {
            const std::lock_guard<std::mutex> lock(mutex_);
            closed_ = true;
        }
        condvar_.notify_all();
    }

    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condvar_.wait(lock, [this]
                      { return closed_ || !queue_.empty(); });
        if (queue_.empty())
            return std::nullopt;
        T value = std::move(queue_.front());
        queue_.pop();
        return value;
    }

private:
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable condvar_;
    bool closed_ = false;
};
