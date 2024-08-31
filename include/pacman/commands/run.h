#pragma once

#include <filesystem>

namespace commands
{

class Run
{
public:
    static void Execute(std::filesystem::path path);
};

} // namespace commands
