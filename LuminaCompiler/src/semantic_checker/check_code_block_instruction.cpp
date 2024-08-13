#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolBodyInstruction>& p_instruction, std::unordered_map<std::string, Type*> p_variables, Type* returnType)
	{
		for (const auto& element : p_instruction->elements)
		{
			try
			{
				switch (element->type)
				{
				case Instruction::Type::VariableDeclaration:
				{
					checkVariableDeclarationInstruction(p_file, std::static_pointer_cast<VariableDeclarationInstruction>(element), p_variables);
					break;
				}
				case Instruction::Type::VariableAssignation:
				{
					checkVariableAssignationInstruction(p_file, std::static_pointer_cast<VariableAssignationInstruction>(element), p_variables);
					break;
				}
				case Instruction::Type::Return:
				{
					checkReturnInstruction(p_file, std::static_pointer_cast<ReturnInstruction>(element), p_variables, returnType);
					break;
				}
				default:
					throw TokenBasedError(p_file, "Unexpected instruction type in symbol body", Token());
				}
			}
			catch (TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}
			
		}
	}
}