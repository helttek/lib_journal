#pragma once

#include <thread>
#include <string>
#include "../src/ThreadSafeQueue.hpp"
#include <fstream>
#include <system_error>

namespace jnl
{

    enum Priority
    {
        LOW,
        MIDDLE,
        HIGH
    };

    constexpr std::string_view to_string(Priority prio)
    {
        switch (prio)
        {
        case jnl::Priority::HIGH:
            return "HIGH";

        case jnl::Priority::MIDDLE:
            return "MIDDLE";

        case jnl::Priority::LOW:
            return "LOW";

        default:
            return "";
        }
    }

    Priority str2prio(std::string str);
}

namespace
{
    class msg
    {
    private:
        std::string text;
        jnl::Priority priority;

    public:
        msg(std::string &text, jnl::Priority priority) : text(text), priority(priority) {}

        jnl::Priority get_priority();

        std::string_view get_text();
    };
}

namespace jnl
{
    class Journal
    {
    private:
        std::string file_name;
        Priority default_priority;
        ThreadSafeQueue<msg> queue;
        std::ofstream file;
        std::thread writer_thread;

        void writer();

    public:
        Journal() = default;

        ~Journal()
        {
            queue.push(Command::THREAD_CANCEL);
        }

        Journal(std::string name) : Journal(name, Priority::HIGH) {}

        Journal(std::string name, Priority priority) : file_name(name), queue(), default_priority(priority), file(file_name, std::ios::out | std::ios::app), writer_thread([this]()
                                                                                                                                                                                { writer(); })
        {
            if (!file.is_open())
            {
                throw std::runtime_error("ERROR: Cannot open log file: " + file_name);
            }
            writer_thread.detach();
        }

        void log(jnl::Priority prio, std::string text);
    };
} // namespace jnl
