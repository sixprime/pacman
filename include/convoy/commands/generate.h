#pragma once

#include <filesystem>
#include <string>

namespace commands
{

class Generate
{
public:
    static void Execute(std::string buildSystem, std::filesystem::path path);
};

} // namespace commands
