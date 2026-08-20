#include <iostream>
#include "libjournal.hpp"
#include <thread>
#include <vector>
#include "ThreadSafeQueue.hpp"
#include <string>
#include <variant>
#include "WorkerThread.hpp"
#include "SignalAction.hpp"

int threads_count = 3;
ThreadSafeQueue<std::unique_ptr<std::string>> queue;

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <journal name> <default message importance>" << std::endl;
        return EXIT_SUCCESS;
    }
    std::string journal_name(argv[1]), default_message_importance(argv[2]);

    try
    {
        setup_sigint_handler();
    }
    catch (const std::exception &e)
    {
        std::cerr << "WARNING: Failed to setup SIGINT handler: " << e.what() << '\n';
    }

    jnl::Priority default_priority;
    if (default_message_importance == "LOW")
    {
        default_priority = jnl::Priority::LOW;
    }
    else if (default_message_importance == "MIDDLE")
    {
        default_priority = jnl::Priority::MIDDLE;
    }
    else if (default_message_importance == "HIGH")
    {
        default_priority = jnl::Priority::HIGH;
    }
    else
    {
        std::cerr << "WARNING: unknown priority '" << default_message_importance
                  << "', using HIGH as default.\n";
        default_priority = jnl::Priority::HIGH;
    }
    jnl::Journal journal(journal_name, default_priority);

    // start threads
    std::vector<std::thread> threads;
    threads.reserve(threads_count);
    for (int i = 0; i < threads_count; i++)
    {
        threads.emplace_back(worker_thread, std::ref(journal), std::ref(queue));
        threads.back().detach();
    }

    while (true)
    {
        // input format: "importance: <importance>\ntext: <message>\n"
        std::string importance, msg;
        std::cout << "importance: ";
        std::getline(std::cin, importance, '\n');
        std::cout << "text: ";
        std::getline(std::cin, msg, '\n');
        std::cout << "------------------------------------------------" << std::endl;
        std::unique_ptr<std::string> message = std::make_unique<std::string>(importance + '\n' + msg);
        queue.push(std::move(message));
    }

    return EXIT_SUCCESS;
}
