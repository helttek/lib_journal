#include "WorkerThread.hpp"

#include <memory>
#include <string>
#include <sstream>
#include <variant>

void worker_thread(jnl::Journal &journal, ThreadSafeQueue<std::unique_ptr<std::string>> &queue)
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

        std::stringstream stream(*std::get<std::unique_ptr<std::string>>(item));
        std::string priority, text;
        std::getline(stream, priority, '\n');
        jnl::Priority prio = jnl::str2prio(priority);
        std::getline(stream, text, '\n');
        journal.log(prio, text);
    }
    return;
}