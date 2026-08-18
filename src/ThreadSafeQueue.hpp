#include <queue>
#include <mutex>
#include <condition_variable>
#include <variant>

template <typename T>
class ThreadSafeQueue
{
public:
    enum Command
    {
        THREAD_CANCEL,
        THREAD_PROCESS
    };

    ThreadSafeQueue() : q(), mutex(), condvar() {}

    void push(const T &value)
    {
        {
            const std::lock_guard<std::mutex> lock(mutex);
            q.push(value);
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

    std::variant<T, Command> front()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condvar.wait(lock, [this]()
                     { return !q.empty(); });
        return q.front();
    }

    std::variant<T, Command> back()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condvar.wait(lock, [this]()
                     { return !q.empty(); });

        return q.back();
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

    void pop()
    {
        std::unique_lock<std::mutex> lock(mutex);
        condvar.wait(lock, [this]()
                     { return !q.empty(); });

        q.pop();
    }

private:
    /** Command - тип команды потоку, например завершиться. */
    std::queue<std::variant<T, Command>> q;
    std::mutex mutex;
    std::condition_variable condvar;
};
