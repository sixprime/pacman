#include "convoy/commands.h"

#include <iostream>
#include <string>
#include <vector>

int main(int argc, char* argv[])
{
    std::vector<std::string> arguments(argv + 1, argv + argc);
    if (arguments.empty())
    {
        std::cout << "convoy help:\n"
            << "  new [options] path            Create new package 'path'\n"
            << "  build [options] path          Build package at 'path'\n"
            << "  run [options] [--args] path   Run package at 'path'\n";

        return 1;
    }

    if (arguments[0] == "new")
    {
        commands::New::Execute(arguments[1]);
    }
    else if (arguments[0] == "build")
    {
        commands::Build::Execute(arguments.size() > 1 ? arguments[1] : ".");
    }
    else if (arguments[0] == "run")
    {
        commands::Run::Execute(arguments.size() > 1 ? arguments[1] : ".");
    }

    return 0;
}
