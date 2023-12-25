#include "commands.h"

#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::vector<std::string> arguments(argv + 1, argv + argc);
    if (arguments.size() >= 2)
    {
        if (arguments[0] == "new")
        {
            commands::New::execute(arguments[1]);
        }
    }

    return 0;
}
