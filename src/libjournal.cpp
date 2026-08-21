#include "../include/libjournal.hpp"

#include <iostream>
#include <ctime>
#include <chrono>
#include <iomanip>

jnl::Priority jnl::str2prio(std::string str)
{
    if (str == "LOW")
    {
        return jnl::Priority::LOW;
    }
    else if (str == "MIDDLE")
    {
        return jnl::Priority::MIDDLE;
    }
    else if (str == "HIGH")
    {
        return jnl::Priority::HIGH;
    }
    else
    {
        return jnl::Priority::HIGH;
    }
}

jnl::Priority msg::get_priority()
{
    return priority;
}

std::string_view msg::get_text()
{
    return text;
}

void jnl::Journal::log(jnl::Priority prio, std::string text)
{
    if (prio < default_priority)
    {
        return;
    }
    queue.push(msg(text, prio));
}

void jnl::Journal::writer()
{
    while (true)
    {
        auto item = queue.pop();
        if (std::holds_alternative<Command>(item))
        {
            if (std::get<Command>(item) == Command::THREAD_CANCEL)
            {
                file.flush();
                break;
            }
        }

        auto now = std::chrono::system_clock::now();
        auto now_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        file << std::put_time(std::localtime(&now_time_t), "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << ms.count()
             << " [" << to_string(std::get<msg>(item).get_priority()) << "]: "
             << std::get<msg>(item).get_text() << std::endl;
    }
    file.flush();
    return;
}
