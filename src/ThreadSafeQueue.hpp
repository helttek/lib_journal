#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <variant>

enum Command
{
    THREAD_CANCEL,
    THREAD_PROCESS
};

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue() : q(), mutex(), condvar() {}

    void push(T &&value)
    {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            q.push(std::move(value));
        }
        condvar.notify_one();
    }

    void push(const Command command)
    {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            q.push(command);
        }
        condvar.notify_one();
    }

    std::variant<T, Command> pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condvar.wait(lock, [this]()
                     { return !q.empty(); });
        auto item = std::move(q.front());
        q.pop();
        return item;
    }

    bool empty() const
    {
        const std::lock_guard<std::mutex> lock(mutex);
        return q.empty();
    }

    typename std::queue<std::variant<T, Command>>::size_type size() const
    {
        const std::lock_guard<std::mutex> lock(mutex);
        return q.size();
    }

private:
    /** Command - тип команды потоку, например завершиться. */
    std::queue<std::variant<T, Command>> q;
    std::mutex mutex;
    std::condition_variable condvar;
};
