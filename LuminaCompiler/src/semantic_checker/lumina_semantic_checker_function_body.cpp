#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolBodyInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType)
	{
		for (const auto& instruction : p_instruction->elements)
		{
			try
			{
				switch (instruction->type)
				{
				case Lumina::Instruction::Type::VariableDeclaration:
				{
					checkVariableDeclarationInstruction(p_file, static_pointer_cast<VariableDeclarationInstruction>(instruction), p_scopeVariables);
					break;
				}
				case Lumina::Instruction::Type::Return:
				{
					checkReturnInstruction(p_file, static_pointer_cast<ReturnInstruction>(instruction), p_scopeVariables, p_expectedReturnType);
					break;
				}
				default:
				{
					throw TokenBasedError(p_file, "Unexpected symbol body instruction type : " + ::to_string(instruction->type) + DEBUG_INFORMATION, Token());
				}
				}
			}
			catch (const TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}
		}
	}

	void SemanticChecker::checkPipelineBodyInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineBodyInstruction>& p_instruction)
	{
		if (p_instruction->pipelineToken.content == "Input")
		{
			throw TokenBasedError(p_file, "Impossible to define pipeline [" + p_instruction->pipelineToken.content + "] body" + DEBUG_INFORMATION, p_instruction->pipelineToken);
		}

		if (p_instruction->pipelineToken.content == "VertexPass")
		{
			if (_vertexPipelineAlreadyParsed == true)
			{
				throw TokenBasedError(p_file, "Pipeline [" + p_instruction->pipelineToken.content + "] already defined" + DEBUG_INFORMATION, p_instruction->pipelineToken);
			}
			_vertexPipelineAlreadyParsed = true;
		}
		else if (p_instruction->pipelineToken.content == "FragmentPass")
		{
			if (_fragmentPipelineAlreadyParsed == true)
			{
				throw TokenBasedError(p_file, "Pipeline [" + p_instruction->pipelineToken.content + "] already defined" + DEBUG_INFORMATION, p_instruction->pipelineToken);
			}
			_fragmentPipelineAlreadyParsed = true;
		}

		checkSymbolBodyInstruction(p_file, p_instruction->body, std::unordered_map<std::string, Variable>(), "void");
	}
}