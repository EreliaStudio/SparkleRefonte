#include "lumina_lexer.hpp"

namespace Lumina
{

	std::shared_ptr<OperatorExpressionInstruction> Lexer::parseOperatorExpressionInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<OperatorExpressionInstruction> result = std::make_shared<OperatorExpressionInstruction>();

		result->token = expect(Lumina::Token::Type::Operator, "Expected an operator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<NumberExpressionValueInstruction> Lexer::parseNumberExpressionValueInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<NumberExpressionValueInstruction> result = std::make_shared<NumberExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::Number, "Expected a number token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<StringLiteralsExpressionValueInstruction> Lexer::parseStringLiteralsExpressionValueInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<StringLiteralsExpressionValueInstruction> result = std::make_shared<StringLiteralsExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::StringLitteral, "Expected a string literal token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<VariableExpressionValueInstruction> Lexer::parseVariableExpressionValueInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<VariableExpressionValueInstruction> result = std::make_shared<VariableExpressionValueInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected a namespace separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		while (currentToken().type == Lumina::Token::Type::Accessor)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::Accessor, "Expected an accessor token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<ExpressionInstruction> Lexer::parseExpression(const std::string& p_debugInformation)
	{
		std::shared_ptr<ExpressionInstruction> result = std::make_shared<ExpressionInstruction>();

		bool isParsing = true;
		while (isParsing)
		{
			switch (currentToken().type)
			{
			case Lumina::Token::Type::Number:
				result->elements.push_back(parseNumberExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				break;
			case Lumina::Token::Type::StringLitteral:
				result->elements.push_back(parseStringLiteralsExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				break;
			case Lumina::Token::Type::Identifier:
			case Lumina::Token::Type::NamespaceSeparator:
			{
				if (describeSymbolCallName() == true)
				{
					result->elements.push_back(parseSymbolCallInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				}
				else
				{
					result->elements.push_back(parseVariableExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				}
				break;
			}
			}

			if (currentToken().type == Lumina::Token::Type::Operator)
			{
				result->elements.push_back(parseOperatorExpressionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				isParsing = true;
			}
			else
			{
				isParsing = false;
			}
		}

		return result;
	}

}