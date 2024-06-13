#pragma once

#include <vector>
#include <filesystem>

std::vector<std::filesystem::path> listFiles(const std::filesystem::path& folderPath)
{
    std::vector<std::filesystem::path> fileList;

    if (!std::filesystem::exists(folderPath) || !std::filesystem::is_directory(folderPath))
    {
        throw std::invalid_argument("The provided path is not a valid directory.");
    }

    for (const auto& entry : std::filesystem::directory_iterator(folderPath))
    {
        if (std::filesystem::is_regular_file(entry.path()))
        {
            fileList.push_back(entry.path());
        }
    }

    return fileList;
}