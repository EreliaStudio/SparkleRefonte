#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<IncludeInstruction> Lexer::parseIncludeInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<IncludeInstruction> result = std::make_shared<IncludeInstruction>();

		expect(Lumina::Token::Type::Include, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->includeFile = expect({ Lumina::Token::Type::StringLitteral, Lumina::Token::Type::IncludeLitteral }, "Expected a valid include file token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return (result);
	}
}