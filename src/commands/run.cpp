#include "convoy/commands/run.h"

#include "convoy/commands/build.h"

#include <array>
#include <string>

using namespace commands;

#include <iostream>

static std::string ExecuteShellCommand(const std::string& command)
{
    std::array<char, 64> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(command.c_str(), "r"), _pclose); // Windows version

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
    const std::string binaryNameWindows = path.stem().string() + ".exe";
    const std::filesystem::path binaryOutputFilepath = path / target / profile / binaryNameWindows;
    const auto result = ExecuteShellCommand("\"\"" + binaryOutputFilepath.string() + "\"\"");
    std::cout << result;
}
