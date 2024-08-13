#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkIfStatementInstruction(const std::filesystem::path& p_file, const std::shared_ptr<IfStatementInstruction>& p_instruction, std::unordered_map<std::string, SemanticChecker::Type*> p_variables, Type* returnType)
	{
		checkExpressionInstruction(p_file, p_instruction->condition->expression, p_variables, type("bool"));

		checkSymbolBodyInstruction(p_file, p_instruction->body, p_variables, returnType);

		for (const auto& elseBlockInstruction : p_instruction->elseBlocks)
		{
			if (elseBlockInstruction->condition != nullptr)
				checkExpressionInstruction(p_file, elseBlockInstruction->condition->expression, p_variables, type("bool"));

			checkSymbolBodyInstruction(p_file, elseBlockInstruction->body, p_variables, returnType);
		}
	}
}