#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkPipelineFlowInstruction(const std::filesystem::path& p_file, const std::shared_ptr<PipelineFlowInstruction>& p_instruction)
	{
		Lumina::Token inputPipeline;
		Lumina::Token outputPipeline;
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;

		if (p_instruction->inputPipeline.content == "Input")
		{
			if (p_instruction->outputPipeline.content != "VertexPass")
			{
				throw TokenBasedError(p_file, "Only pipeline flow acceptable for [Input] input is [VertexPass]" + DEBUG_INFORMATION, p_instruction->outputPipeline);
			}
		}
		else if (p_instruction->inputPipeline.content == "VertexPass")
		{
			if (p_instruction->outputPipeline.content != "FragmentPass")
			{
				throw TokenBasedError(p_file, "Only pipeline flow acceptable for [VertexPass] input is [FragmentPass]" + DEBUG_INFORMATION, p_instruction->outputPipeline);
			}
		}
		else
		{
			throw TokenBasedError(p_file, "Pipeline flow entry can only be [Input] or [VertexPass]" + DEBUG_INFORMATION, p_instruction->inputPipeline);
		}

		if (_pipelineAllowedTypes.contains(p_instruction->type->string()) == false)
		{
			if (_pipelineAllowedTypes.contains(p_instruction->type->string().substr(2, p_instruction->type->string().size() - 2)) == false)
			{
				throw TokenBasedError(p_file, "Type [" + p_instruction->type->string() + "] not accepted as pipeline flow type" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
			}
		}
		if (_pipelineFlowUsedNames.contains(p_instruction->name.content) == true)
		{
			throw TokenBasedError(p_file, "Pipeline flow [" + p_instruction->name.content + "] variable already created" + DEBUG_INFORMATION, p_instruction->name);
		}
		_pipelineFlowUsedNames.insert(p_instruction->name.content);
	}
}