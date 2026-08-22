#include "Application.hpp"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <optional>
#include <string>

#include "libjournal.hpp"

namespace
{

    int printUsage(const char *program_name)
    {
        std::cerr << "Usage: " << program_name
                  << " <journal file name> <default priority>\n"
                  << "Input message format: \"[PRIORITY] message\"; priority can be omitted.\n";
        return EXIT_FAILURE;
    }

} // namespace

int main(int argc, char *argv[])
{
    if (argc != 3)
        return printUsage(argv[0]);

    const std::optional<jnl::Priority> defaultPriority = jnl::parse_priority(argv[2]);
    if (!defaultPriority)
    {
        std::cerr << "ERROR: Unknown priority \"" << argv[2] << "\"\n";
        return printUsage(argv[0]);
    }

    try
    {
        Application app(argv[1], *defaultPriority);
        return app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << "ERROR: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
}
