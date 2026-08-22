#include "libjournal.hpp"

#include "ThreadSafeQueue.hpp"

#include <atomic>
#include <chrono>
#include <cctype>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <thread>
#include <utility>

namespace jnl
{

    std::string_view to_string(Priority priority) noexcept
    {
        switch (priority)
        {
        case Priority::LOW:
            return "LOW";
        case Priority::MIDDLE:
            return "MIDDLE";
        case Priority::HIGH:
            return "HIGH";
        }
        return "";
    }

    std::optional<Priority> parse_priority(const std::string &name)
    {
        std::string upper;
        upper.reserve(name.size());
        for (const char c : name)
            upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));

        if (upper == "LOW")
            return Priority::LOW;
        if (upper == "MIDDLE")
            return Priority::MIDDLE;
        if (upper == "HIGH")
            return Priority::HIGH;
        return std::nullopt;
    }

    namespace
    {
        struct Entry
        {
            std::chrono::system_clock::time_point received_at;
            Priority priority;
            std::string text;
        };

        std::string format_time(std::chrono::system_clock::time_point tp)
        {
            const auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(tp);
            const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(tp - seconds).count();

            const std::time_t time = std::chrono::system_clock::to_time_t(seconds);
            std::tm local{};
            localtime_r(&time, &local);

            std::ostringstream out;
            out << std::put_time(&local, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3)
                << milliseconds;
            return out.str();
        }

    } // namespace

    struct Journal::Impl
    {
        Impl(const std::string &name, Priority default_priority)
            : file_name_(name), default_priority_(default_priority)
        {
            file_.open(name, std::ios::out | std::ios::app);
            if (!file_.is_open())
                throw std::runtime_error("Cannot open journal file: " + name);

            writer_thread_ = std::thread([this]
                                         { write_loop(); });
        }

        ~Impl()
        {
            queue_.close();
            writer_thread_.join();
        }

        void log(Priority priority, const std::string &text)
        {
            if (priority < default_priority_.load(std::memory_order_relaxed))
                return;
            queue_.push(Entry{std::chrono::system_clock::now(), priority, text});
        }

        bool good() const noexcept
        {
            return !write_failed_.load(std::memory_order_relaxed);
        }

        Priority default_priority() const noexcept
        {
            return default_priority_.load(std::memory_order_relaxed);
        }

        void set_default_priority(Priority priority) noexcept
        {
            default_priority_.store(priority, std::memory_order_relaxed);
        }

    private:
        void write_loop()
        {
            while (auto entry = queue_.pop())
            {
                file_ << format_time(entry->received_at) << " [" << to_string(entry->priority) << "] "
                      << entry->text << '\n';
                file_.flush();
                check_write_state();
            }
        }

        void check_write_state()
        {
            if (!file_ && !write_failed_.exchange(true))
                std::cerr << "libjournal: failed write to file  \"" << file_name_ << "\"\n";
        }

        std::string file_name_;
        ThreadSafeQueue<Entry> queue_;
        std::ofstream file_;
        std::atomic<Priority> default_priority_;
        std::atomic<bool> write_failed_{false};
        std::thread writer_thread_;
    };

    Journal::Journal(const std::string &name, Priority default_priority)
        : impl_(std::make_unique<Impl>(name, default_priority))
    {
    }

    Journal::~Journal() = default;

    void Journal::log(Priority priority, std::string text)
    {
        impl_->log(priority, text);
    }

    void Journal::log(std::string text)
    {
        impl_->log(impl_->default_priority(), std::move(text));
    }

    void Journal::set_default_priority(Priority priority) noexcept
    {
        impl_->set_default_priority(priority);
    }

    Priority Journal::get_default_priority() const noexcept
    {
        return impl_->default_priority();
    }

    bool Journal::good() const noexcept
    {
        return impl_->good();
    }

} // namespace jnl
