#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<SymbolParameterInstruction> Lexer::parseSymbolParameterInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<SymbolParameterInstruction> result = std::make_shared<SymbolParameterInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolNameInstruction> Lexer::parseSymbolCallNameInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<SymbolNameInstruction> result = std::make_shared<SymbolNameInstruction>();

		if (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected a namespace separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<SymbolCallInstruction> Lexer::parseSymbolCallInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<SymbolCallInstruction> result = std::make_shared<SymbolCallInstruction>();

		result->name = parseSymbolCallNameInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			if (result->arguments.size() != 0)
			{
				expect(Lumina::Token::Type::Comma, "Expected a comma." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
			result->arguments.push_back(parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}