#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<ConditionInstruction> Lexer::parseConditionInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<ConditionInstruction> result = std::make_shared<ConditionInstruction>();

		result->expression = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<IfStatementInstruction> Lexer::parseIfStatementInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<IfStatementInstruction> result = std::make_shared<IfStatementInstruction>();

		expect(Lumina::Token::Type::IfStatement, "Expected an 'if' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		while (currentToken().type == Lumina::Token::Type::ElseStatement)
		{
			expect(Lumina::Token::Type::ElseStatement, "Expected an 'else' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

			std::shared_ptr<ElseInstruction> elseInstr = std::make_shared<ElseInstruction>();

			if (currentToken().type == Lumina::Token::Type::IfStatement)
			{
				expect(Lumina::Token::Type::IfStatement, "Expected an 'if' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				elseInstr->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}

			elseInstr->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->elseBlocks.push_back(elseInstr);
		}

		return result;
	}

	std::shared_ptr<WhileLoopInstruction> Lexer::parseWhileLoopInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<WhileLoopInstruction> result = std::make_shared<WhileLoopInstruction>();

		expect(Lumina::Token::Type::WhileStatement, "Expected a 'while' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ForLoopInstruction> Lexer::parseForLoopInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<ForLoopInstruction> result = std::make_shared<ForLoopInstruction>();

		expect(Lumina::Token::Type::ForStatement, "Expected a 'for' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			if (describeVariableDeclarationInstruction())
			{
				result->initializer = parseVariableDeclarationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
			else
			{
				result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			result->increment = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}