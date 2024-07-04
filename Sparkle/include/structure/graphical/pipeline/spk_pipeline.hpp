#pragma once

#include <string>
#include <filesystem>

namespace spk
{
	class Pipeline
	{
	private:

	public:
		Pipeline();

		void loadFromString(const std::string& p_inputCode);
		void load(const std::filesystem::path& p_path);
	};
}