#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkNamespaceInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NamespaceInstruction>& p_instruction)
	{
		size_t namespaceIndex = 0;

		_currentNamespace.push_back(p_instruction->name);

		std::string namespacePrefix = "";
		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				namespacePrefix += "::";
			namespacePrefix += _currentNamespace[i].content;
		}

		while (namespaceIndex < p_instruction->instructions.size())
		{
			try
			{
				std::shared_ptr<AbstractInstruction> instruction = p_instruction->instructions[namespaceIndex];

				switch (instruction->type)
				{
				case Instruction::Type::Namespace:
				{
					checkNamespaceInstruction(p_file, static_pointer_cast<NamespaceInstruction>(instruction));
					break;
				}
				default:
				{
					throw TokenBasedError(p_file, "Unexpected instruction type inside namespace [" + namespacePrefix + "] : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
				}
				}
			}
			catch (const TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}

			namespaceIndex++;
		}

		_currentNamespace.pop_back();
	}
}