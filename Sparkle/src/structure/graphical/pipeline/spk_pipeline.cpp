#include "structure/graphical/pipeline/spk_pipeline.hpp"

#include <fstream>


namespace spk
{
	Pipeline::Pipeline()
	{
	}

	void Pipeline::loadFromString(const std::string& p_inputCode)
	{

	}

	void Pipeline::load(const std::filesystem::path& p_path)
	{
		std::fstream inputFile;
		inputFile.open("shader/shader.lum", std::ios_base::in);

		std::string line;
		std::string code = "";
		while (std::getline(inputFile, line))
		{
			code += line + "\n";
		}

		inputFile.close();

		loadFromString(code);
	}
}