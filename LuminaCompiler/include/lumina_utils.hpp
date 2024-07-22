#pragma once

#ifndef NDEBUG
#define DEBUG_INFORMATION std::string(" ") + __FUNCTION__ + "::" + std::to_string(__LINE__)
#else
#define DEBUG_INFORMATION std::string()
#endif

#define DEBUG_LINE() std::cout << __FUNCTION__ << "::" << __LINE__ << std::endl

#include <string>
#include <filesystem>

namespace Lumina
{
	std::string readFileAsString(const std::filesystem::path& p_path);
	std::filesystem::path composeFilePath(const std::string& fileName);
}