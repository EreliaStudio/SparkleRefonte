#pragma once

#include <vector>
#include <filesystem>

namespace spk
{
	namespace FileUtils
	{
		std::vector<std::filesystem::path> listFiles(const std::filesystem::path& folderPath);
	}
}