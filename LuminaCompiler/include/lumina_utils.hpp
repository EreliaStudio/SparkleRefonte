#pragma once

#include <string>
#include <filesystem>

std::string readFileAsString(const std::filesystem::path& p_path);
std::filesystem::path composeFilePath(const std::string& fileName);