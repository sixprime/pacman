#pragma once

#include <filesystem>
#include <string>

namespace commands
{

class Generate
{
public:
    static void Execute(std::string buildSystem, std::filesystem::path path, std::filesystem::path target = "bin", std::filesystem::path profile = "debug");
};

} // namespace commands
