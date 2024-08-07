#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	SemanticChecker::Result SemanticChecker::checkSemantic(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
	{
		return (SemanticChecker().check(p_file, p_instructions));
	}

	void SemanticChecker::setup()
	{
		setupTypes();
		setupAllowedPipelineTypes();
	}

	SemanticChecker::Result SemanticChecker::check(const std::filesystem::path& p_file, std::vector<std::shared_ptr<AbstractInstruction>>& p_instructions)
	{
		_result = Result();
		_index = 0;

		setup();

		for (const auto& [name, type] : _availableTypes)
		{
			std::cout << "Type : " << type << std::endl;
		}

		for (auto& instruction : p_instructions)
		{
			_elements.push_back(Element{ p_file, instruction });
		}

		while (_index < _elements.size())
		{
			auto& element = _elements[_index];
			try
			{
				std::shared_ptr<AbstractInstruction> instruction = element.instruction;

				switch (instruction->type)
				{
				case Instruction::Type::Include:
				{
					checkIncludeInstruction(element.filePath, static_pointer_cast<IncludeInstruction>(instruction));
					break;
				}
				default:
				{
					throw TokenBasedError(element.filePath, "Unexpected instruction type : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
				}
				}
			}
			catch (const TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}

			_index++;
		}

		return _result;
	}
}