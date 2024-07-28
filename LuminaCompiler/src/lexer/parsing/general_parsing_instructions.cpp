#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<TypeInstruction> Lexer::parseTypeInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<TypeInstruction> result = std::make_shared<TypeInstruction>();

		if (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<IdentifierInstruction> Lexer::parseIdentifierInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<IdentifierInstruction> result = std::make_shared<IdentifierInstruction>();

		result->token = expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}