#pragma once

#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace jnl
{

    enum class Priority
    {
        LOW = 0,
        MIDDLE = 1,
        HIGH = 2
    };

    std::string_view to_string(Priority priority) noexcept;

    std::optional<Priority> parse_priority(const std::string &name);

    class Journal
    {
    public:
        Journal(const std::string &name, Priority default_priority);

        explicit Journal(const std::string &name);

        ~Journal();

        Journal(const Journal &) = delete;
        Journal &operator=(const Journal &) = delete;

        void log(Priority priority, std::string text);

        void log(std::string text);

        void set_default_priority(Priority priority) noexcept;

        Priority get_default_priority() const noexcept;

        bool good() const noexcept;

    private:
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };

} // namespace jnl
