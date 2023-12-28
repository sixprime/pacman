#include "convoy/commands/build.h"

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

    std::ifstream file { path / "convoy.toml" };
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

static std::string MakeClangCompilationCommandLine(std::filesystem::path path, const Manifest& manifest, const std::vector<std::filesystem::path>& srcFiles)
{
    // Ex: "clang++ -std=c++17 -Wall -Wextra -Wpedantic -Isrc/ src/main.cpp src/commands/new.cpp src/commands/build.cpp -o convoy.exe"
    std::filesystem::path publicIncludeDir = path / "include";
    std::filesystem::path privateIncludeDir = path / "src";
    std::string command = "clang++ -std=c++17 -Wall -Wextra -Wpedantic -I" + publicIncludeDir.string() + " -I" + privateIncludeDir.string();

    for (const auto& srcFile : srcFiles)
    {
        command += " " + srcFile.string() + " ";
    }

    const auto packageBinaryOutput = manifest.packageName + ".exe";
    command += " -o " + packageBinaryOutput;

    return command;
}

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

void Build::Execute(std::filesystem::path path)
{
    auto start = std::chrono::high_resolution_clock::now();

    // Read package name and version from manifest file.
    Manifest manifest = GetManifestInfo(path);
    if (manifest.packageName.empty() || manifest.packageVersion.empty())
    {
        std::cerr << "Failed to find manifest package name or version..." << std::endl;
        return;
    }

    std::cout << "Compiling " << manifest.packageName << " v" << manifest.packageVersion << std::endl;

    // Compile.
    std::vector<std::filesystem::path> srcFiles = GetFilesToCompile(path / "src");
    const std::string compilationCommand = MakeClangCompilationCommandLine(path, manifest, srcFiles);
    const std::string result = ExecuteShellCommand(compilationCommand);
    if (!result.empty())
    {
        std::cerr << result << std::endl;
    }

    // Move binary to target folder.
    const std::filesystem::path target = "bin";
    const std::filesystem::path profile = "debug";
    const std::filesystem::path outputDirectory = std::filesystem::current_path() / path / target / profile;
    std::filesystem::create_directories(outputDirectory);
    const std::string binaryNameWindows = manifest.packageName + ".exe";
    std::filesystem::rename(binaryNameWindows, path / target / profile / binaryNameWindows);

    auto stop = std::chrono::high_resolution_clock::now();
    auto durationSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Finished " << target.string() << " [" << profile.string() << "] target in " << durationSeconds.count() / 1000.0 << " secs" << std::endl;
}
