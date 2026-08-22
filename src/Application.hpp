#pragma once

#include "ThreadSafeQueue.hpp"
#include "libjournal.hpp"

#include <optional>
#include <string>
#include <thread>
#include <vector>

class Application
{
public:
    static constexpr std::size_t kWorkerCount = 3;

    Application(const std::string &journalName, jnl::Priority defaultPriority);

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    int run();

private:
    struct Task
    {
        std::optional<jnl::Priority> priority;
        std::string text;
    };

    void workerLoop();
    void handleInputLine(const std::string &line);

    jnl::Journal journal_;
    ThreadSafeQueue<Task> tasks_;
    std::vector<std::thread> workers_;
};
