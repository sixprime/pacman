#pragma once

#include <filesystem>

namespace commands
{

class New
{
public:
    static void Execute(std::filesystem::path path);
};

} // namespace commands
