#pragma once

#include <filesystem>

namespace commands
{

class New
{
public:
    static void execute(std::filesystem::path path);
};

} // namespace commands
