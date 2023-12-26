#include "commands/new.h"

#include <fstream>

using namespace commands;

static void CreateDefaultManifest(std::filesystem::path path)
{
    const auto manifest = std::filesystem::current_path() / path / "convoy.toml";
    const auto directory = manifest.parent_path();
    std::filesystem::create_directories(directory);

    std::ofstream file { manifest };
    if (file.is_open())
    {
        file << "[package]\nname = " << path.stem() << "\nversion = \"0.1.0\"\n\n[dependencies]\n";
        file.close();
    }
}

static void CreateDefaultSourceFile(std::filesystem::path path)
{
    const auto filepath = std::filesystem::current_path() / path / "src" / "main.cpp";
    const auto directory = filepath.parent_path();
    std::filesystem::create_directories(directory);

    std::ofstream file { filepath };
    if (file.is_open())
    {
        file << "#include <iostream>\n\nint main()\n{\n\tstd::cout<< \"Hello World!\" << std::endl;\n}\n";
        file.close();
    }
}

void New::Execute(std::filesystem::path path)
{
    CreateDefaultManifest(path);
    CreateDefaultSourceFile(path);
}
