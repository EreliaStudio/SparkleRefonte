#include "lumina_utils.hpp"

#include <fstream>
#include <iostream>

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

std::filesystem::path composeFilePath(const std::string& fileName)
{
	std::string pathStr;

#ifdef _WIN32
	char* pathEnv = nullptr;
	size_t len = 0;
	if (_dupenv_s(&pathEnv, &len, "Path") != 0 || pathEnv == nullptr)
	{
		std::cerr << "PATH environment variable not found." << std::endl;
		return std::filesystem::path();
	}
	pathStr = std::string(pathEnv);
	free(pathEnv);
#else
	const char* pathEnv = std::getenv("PATH");
	if (!pathEnv)
	{
		std::cerr << "PATH environment variable not found." << std::endl;
		return std::filesystem::path();
	}
	pathStr = std::string(pathEnv);
#endif

	std::vector<std::filesystem::path> paths;
	std::stringstream ss(pathStr);
	std::string path;

#ifdef _WIN32
	const char delimiter = ';';
#else
	const char delimiter = ':';
#endif

	while (std::getline(ss, path, delimiter))
	{
		paths.push_back(path);
	}

	for (const auto& dir : paths)
	{
		std::filesystem::path filePath = dir / fileName;
		if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
		{
			return filePath;
		}
	}

	std::filesystem::path currentDir = std::filesystem::current_path();
	
	std::filesystem::path filePath = currentDir / fileName;
	if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
	{
		return filePath;
	}

	return std::filesystem::path();
}