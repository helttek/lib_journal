#include <iostream>
#include <csignal>
#include "libjournal.hpp"

extern "C" void signal_handler(int signal_number)
{
    if (signal_number == SIGINT)
    {
        // stop the program
    }
}

int threads_count = 3;

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        std::cout << "Usage: " << argv[0] << " <journal name> <default message importance>" << std::endl;
        return EXIT_SUCCESS;
    }
    std::string journal_name(argv[1]), default_message_importance(argv[2]);

    // load the library + create libjournal object
    Journal journal;

    // setup signal handler
    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    // if (sigaction(SIGINT, &action, nullptr) < 0)
    // {
    //     std::cerr << "WARNING: Failed to setup SIGINT handler." << std::endl;
    //     return 1;
    // }

    for (int i = 0; i < threads_count; i++)
    {
        // init threads that will be adding messages to journal
    }

    // start listening for user input
    while (true)
    {
        // input format: importance: <importance>\nmessage: <message>\n
        // use unique pointer and move to not block the main thread
    }

    return EXIT_SUCCESS;
}
