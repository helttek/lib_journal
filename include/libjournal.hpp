#pragma once

#include <thread>
#include <string>
#include "../src/ThreadSafeQueue.hpp"

namespace jnl
{
    enum Priority
    {
        LOW,
        MIDDLE,
        HIGH
    };

}

namespace
{
    class msg
    {
    private:
        std::string text;
        jnl::Priority priority;

    public:
        msg(std::string text, jnl::Priority priority) : text(text), priority(priority) {}

        jnl::Priority get_priority();
    };
}

namespace jnl
{
    class Journal
    {
    private:
        ThreadSafeQueue<msg> queue;
        std::vector<std::thread> threads;
        size_t num_threads;
        std::string file_name;
        Priority default_priority;

        void writer();

    public:
        Journal() = default;

        Journal(std::string name) : Journal(name, Priority::HIGH) {}

        Journal(std::string name, Priority priority) : num_threads(1), file_name(name), threads(num_threads), queue(), default_priority(priority)
        {
            threads.reserve(num_threads);
            for (size_t i = 0; i < num_threads; i++)
            {
                threads.emplace_back([this]()
                                     { writer(); });
                threads.back().detach();
            }
        }

        void log();

        void stop();
    };
} // namespace jnl
