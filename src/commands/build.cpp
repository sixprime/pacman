#include "pacman/commands/build.h"

#include "pacman/commands/generate.h"

#include <array>
#include <chrono>
#include <cstdio>
#include <iostream>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

using namespace commands;
using namespace std::string_literals;

struct Manifest
{
    std::string packageName;
    std::string packageVersion;
};

static Manifest GetManifestInfo(std::filesystem::path path)
{
    Manifest manifest;

    std::ifstream file { path / "pacman.toml" };
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            const auto delimiter = " = "s;
            auto pos = line.find(delimiter);
            if (pos != std::string::npos)
            {
                auto key = line.substr(0, pos);
                auto value = line.substr(pos + delimiter.size(), line.size());

                if (key == "name")
                {
                    manifest.packageName = value.substr(1, value.size() - 2); // Remove quotes.
                }
                else if (key == "version")
                {
                    manifest.packageVersion = value.substr(1, value.size() - 2); // Remove quotes.
                }
            }
        }
        file.close();
    }

    return manifest;
}

#if 0
static std::vector<std::filesystem::path> GetFilesToCompile(std::filesystem::path path)
{
    std::vector<std::filesystem::path> files;

    for (const auto& it: std::filesystem::recursive_directory_iterator(path))
    {
        const auto& path = it.path();
        if (path.extension() == ".cpp")
        {
            files.emplace_back(path);
        }
    }

    return files;
}

static std::string MakeClangCompilationCommandLine(std::filesystem::path path, const Manifest& manifest, const std::vector<std::filesystem::path>& srcFiles, std::filesystem::path target, std::filesystem::path profile)
{
    std::filesystem::path publicIncludeDir = path / "include";
    std::filesystem::path privateIncludeDir = path / "src";
    std::string command = "clang++ -std=c++17 -Wall -Wextra -Wpedantic -I" + publicIncludeDir.string() + " -I" + privateIncludeDir.string();

    for (const auto& srcFile : srcFiles)
    {
        command += " " + srcFile.string() + " ";
    }

    std::filesystem::path output = path / target / profile / manifest.packageName;
#ifdef _WIN32
    output += ".exe";
#endif // _WIN32

    command += " -o " + output.string();

    return command;
}
#endif

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

void Build::Execute(std::filesystem::path path)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Read package name and version from manifest file.
    Manifest manifest = GetManifestInfo(path);

    if (manifest.packageName.empty())
    {
        std::cerr << "pacman: failed to find manifest package name!" << std::endl;
        return;
    }

    if (manifest.packageVersion.empty())
    {
        std::cerr << "pacman: failed to find manifest package version!" << std::endl;
        return;
    }

    // Compile.
    std::cout << "pacman: compiling " << manifest.packageName << " v" << manifest.packageVersion << std::endl;

    const std::filesystem::path target = "bin";
    const std::filesystem::path profile = "debug";
    Generate::Execute("ninja", path, target, profile);

#if 0
    const std::filesystem::path outputDirectory = std::filesystem::current_path() / path / target / profile;
    std::filesystem::create_directories(outputDirectory);

    std::vector<std::filesystem::path> srcFiles = GetFilesToCompile(path / "src");
    const std::string compilationCommand = MakeClangCompilationCommandLine(path, manifest, srcFiles, target, profile);
#endif

    const std::string compilationCommand = "ninja -C " + path.string();
    const std::string result = ExecuteShellCommand(compilationCommand);
    if (!result.empty())
    {
        std::cout << result;
    }

    auto stop = std::chrono::high_resolution_clock::now();
    auto durationSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "pacman: finished " << target.string() << " [" << profile.string() << "] target in " << durationSeconds.count() / 1000.0 << " secs" << std::endl;
}
