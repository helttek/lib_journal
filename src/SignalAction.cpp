#include "SignalAction.hpp"
#include <cstdlib>
#include <iostream>

extern "C" void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        for (int i = 0; i < threads_count; i++)
        {
            queue.push(Command::THREAD_CANCEL);
        }

        std::exit(EXIT_SUCCESS);
    }
}

void setup_sigint_handler()
{
    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGINT, &action, nullptr) < 0)
    {
        throw std::system_error(errno, std::generic_category());
    }
}