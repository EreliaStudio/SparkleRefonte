#include "utils/spk_file_utils.hpp"

#include <fstream>

namespace spk
{
	namespace FileUtils
	{
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

		std::wstring readFileAsWString(const std::filesystem::path& p_path)
		{
			std::wfstream inputFile;
			inputFile.open(p_path, std::ios_base::in);

			std::wstring line;
			std::wstring result = L"";
			while (std::getline(inputFile, line))
			{
				result += line + L"\n";
			}

			inputFile.close();

			return (result);
		}

		std::string readFileAsString(const std::filesystem::path& p_path)
		{
			std::fstream inputFile;
			inputFile.open(p_path, std::ios_base::in);

			std::string line;
			std::string result = "";
			while (std::getline(inputFile, line))
			{
				result += line + "\n";
			}

			inputFile.close();

			return (result);
		}
	}
}