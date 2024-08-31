#pragma once

#include <filesystem>

namespace commands
{

class Build
{
public:
    static void Execute(std::filesystem::path path);
};

} // namespace commands
