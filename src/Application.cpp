#include "Application.hpp"

#include <cstdlib>
#include <iostream>
#include <utility>

namespace
{

    std::string trim(const std::string &s)
    {
        const auto begin = s.find_first_not_of(" \t");
        if (begin == std::string::npos)
            return {};
        const auto end = s.find_last_not_of(" \t");
        return s.substr(begin, end - begin + 1);
    }

} // namespace

Application::Application(const std::string &journalName, const jnl::Priority defaultPriority)
    : journal_(journalName, defaultPriority)
{
    workers_.reserve(kWorkerCount);
}

int Application::run()
{
    for (std::size_t i = 0; i < kWorkerCount; ++i)
        workers_.emplace_back([this]
                              { workerLoop(); });

    int exitCode = EXIT_SUCCESS;
    std::string line;
    while (true)
    {
        std::cout << "> " << std::flush;
        if (!std::getline(std::cin, line))
        {
            if (std::cin.eof())
                std::cout << std::endl
                          << "EOF reached." << std::endl;
            else
            {
                std::cerr << "ERROR: Failed to read the line from cin." << std::endl;
                exitCode = EXIT_FAILURE;
            }
            break;
        }
        handleInputLine(line);
    }

    // clean up
    tasks_.close();
    for (std::thread &worker : workers_)
        worker.join();
    workers_.clear();
    return exitCode;
}

void Application::workerLoop()
{
    while (auto task = tasks_.pop())
    {
        if (task->priority)
            journal_.log(*task->priority, std::move(task->text));
        else
            journal_.log(std::move(task->text));
    }
}

void Application::handleInputLine(const std::string &line)
{
    const std::string input = trim(line);
    if (input.empty())
        return;

    Task task;

    if (input.front() == '[')
    {
        const auto closing = input.find(']');
        if (closing == std::string::npos)
        {
            std::cerr << "ERROR: no closing ]" << std::endl;
            return;
        }
        const std::string level = trim(input.substr(1, closing - 1));
        const std::optional<jnl::Priority> parsed = jnl::parse_priority(level);
        if (!parsed)
        {
            std::cerr << "WARNING: unknown priority \"" << level
                      << "\" (can be LOW, MIDDLE or HIGH), skipping the message." << std::endl;
            return;
        }
        task.priority = *parsed;
        task.text = trim(input.substr(closing + 1));
    }
    else
    {
        task.text = input;
    }

    if (task.text.empty())
    {
        return;
    }

    tasks_.push(std::move(task));
}
