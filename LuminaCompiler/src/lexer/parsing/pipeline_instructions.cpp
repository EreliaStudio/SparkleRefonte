#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<PipelineFlowInstruction> Lexer::parsePipelineFlowInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<PipelineFlowInstruction> result = std::make_shared<PipelineFlowInstruction>();

		result->inputPipeline = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::PipelineFlowSeparator, "Expected a pipeline flow separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->outputPipeline = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::Separator, "Expected a separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<PipelineBodyInstruction> Lexer::parsePipelineBodyInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<PipelineBodyInstruction> result = std::make_shared<PipelineBodyInstruction>();

		result->pipelineToken = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}