#include "convoy/commands.h"

#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::vector<std::string> arguments(argv + 1, argv + argc);
    if (arguments.size() >= 2)
    {
        if (arguments[0] == "new")
        {
            commands::New::Execute(arguments[1]);
        }
        else if (arguments[0] == "build")
        {
            commands::Build::Execute(arguments[1]);
        }
        else if (arguments[0] == "run")
        {
            commands::Run::Execute(arguments[1]);
        }
    }

    return 0;
}
