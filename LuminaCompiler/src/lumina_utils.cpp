#include "lumina_utils.hpp"

#include <fstream>

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