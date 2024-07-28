#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<BlockElementInstruction> Lexer::parseBlockElementInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<BlockElementInstruction> result = std::make_shared<BlockElementInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<StructureBlockInstruction> Lexer::parseStructureBlockInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<StructureBlockInstruction> result = std::make_shared<StructureBlockInstruction>();

		expect(Lumina::Token::Type::StructureBlock, "Expected a structure block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<AttributeBlockInstruction> Lexer::parseAttributeBlockInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<AttributeBlockInstruction> result = std::make_shared<AttributeBlockInstruction>();

		expect(Lumina::Token::Type::AttributeBlock, "Expected an attribute block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ConstantBlockInstruction> Lexer::parseConstantBlockInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<ConstantBlockInstruction> result = std::make_shared<ConstantBlockInstruction>();

		expect(Lumina::Token::Type::ConstantBlock, "Expected a constant block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);

				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<TextureInstruction> Lexer::parseTextureInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<TextureInstruction> result = std::make_shared<TextureInstruction>();

		expect(Lumina::Token::Type::Texture, "Expected a texture token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}