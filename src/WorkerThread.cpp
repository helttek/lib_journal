#include "WorkerThread.hpp"

void worker_thread(Journal &journal, ThreadSafeQueue<std::unique_ptr<std::string>> &queue)
{
    while (true)
    {
        auto item = queue.pop();
        if (std::holds_alternative<Command>(item))
        {
            if (std::get<Command>(item) == Command::THREAD_CANCEL)
            {
                break;
            }
        }

        // process msg
    }
    return;
}