#include "pacman/commands/run.h"

#include "pacman/commands/build.h"

#include <array>
#include <string>

using namespace commands;

#include <iostream>

static std::string ExecuteShellCommand(const std::string& command)
{
    std::array<char, 64> buffer;
    std::string result;
#ifdef _WIN32
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose);
#else // !_WIN32
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command.c_str(), "r"), pclose);
#endif // _WIN32

    if (!pipe)
    {
        return "popen() failed!";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}

void Run::Execute(std::filesystem::path path)
{
    Build::Execute(path);

    const std::filesystem::path target = "bin";
    const std::filesystem::path profile = "debug";

    if (path == ".")
    {
        path = std::filesystem::current_path();
    }

    std::string binaryTarget = path.stem().string();
#ifdef _WIN32
    binaryTarget += ".exe";
#endif // _WIN32

    const std::filesystem::path binaryTargetFilepath = path / target / profile / binaryTarget;
    const auto result = ExecuteShellCommand("\"\"" + binaryTargetFilepath.string() + "\"\"");
    std::cout << result;
}
