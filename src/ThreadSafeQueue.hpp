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
        const std::unique_lock<std::mutex> lock(mutex);
        if (q.empty())
        {
            condvar.wait();
        }
        
        return q.front();
    }

    std::variant<T, Command> back()
    {
        const std::unique_lock<std::mutex> lock(mutex);
        if (q.empty())
        {
            condvar.wait();
        }
        
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

private:
    /** Command - тип команды потоку, например завершиться. */
    std::queue<std::variant<T, Command>> q;
    std::mutex mutex;
    std::condition_variable condvar;
};
