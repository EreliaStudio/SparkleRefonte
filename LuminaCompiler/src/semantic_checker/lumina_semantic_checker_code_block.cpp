#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolBodyInstruction>& p_instruction, std::unordered_map<std::string, Type*> p_variables)
	{
		std::cout << "Availible variable : " << std::endl;
		for (const auto& [key, type] : p_variables)
		{
			std::cout << "	- " << key << " - " << type->name << std::endl;
		}
	}
}