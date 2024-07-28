#include "lumina_lexer.hpp"

namespace Lumina
{

	std::shared_ptr<OperatorExpressionInstruction> Lexer::parseOperatorExpressionInstruction()
	{
		std::shared_ptr<OperatorExpressionInstruction> result = std::make_shared<OperatorExpressionInstruction>();

		result->token = expect(Lumina::Token::Type::Operator, "Expected an operator token."+ DEBUG_INFORMATION);

		return result;
	}

	std::shared_ptr<NumberExpressionValueInstruction> Lexer::parseNumberExpressionValueInstruction()
	{
		std::shared_ptr<NumberExpressionValueInstruction> result = std::make_shared<NumberExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::Number, "Expected a number token."+ DEBUG_INFORMATION);

		return result;
	}

	std::shared_ptr<StringLiteralsExpressionValueInstruction> Lexer::parseStringLiteralsExpressionValueInstruction()
	{
		std::shared_ptr<StringLiteralsExpressionValueInstruction> result = std::make_shared<StringLiteralsExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::StringLitteral, "Expected a string literal token."+ DEBUG_INFORMATION);

		return result;
	}

	std::shared_ptr<VariableExpressionValueInstruction> Lexer::parseVariableExpressionValueInstruction()
	{
		std::shared_ptr<VariableExpressionValueInstruction> result = std::make_shared<VariableExpressionValueInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token."+ DEBUG_INFORMATION));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected a namespace separator token."+ DEBUG_INFORMATION));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token."+ DEBUG_INFORMATION));
		}
		while (currentToken().type == Lumina::Token::Type::Accessor)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::Accessor, "Expected an accessor token."+ DEBUG_INFORMATION));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token."+ DEBUG_INFORMATION));
		}

		return result;
	}

	std::shared_ptr<ExpressionInstruction> Lexer::parseExpression()
	{
		std::shared_ptr<ExpressionInstruction> result = std::make_shared<ExpressionInstruction>();

		bool isParsing = true;
		while (isParsing)
		{
			switch (currentToken().type)
			{
			case Lumina::Token::Type::Number:
				result->elements.push_back(parseNumberExpressionValueInstruction());
				break;
			case Lumina::Token::Type::StringLitteral:
				result->elements.push_back(parseStringLiteralsExpressionValueInstruction());
				break;
			case Lumina::Token::Type::Identifier:
			case Lumina::Token::Type::NamespaceSeparator:
			{
				if (describeSymbolCallName() == true)
				{
					result->elements.push_back(parseSymbolCallInstruction());
				}
				else
				{
					result->elements.push_back(parseVariableExpressionValueInstruction());
				}
				break;
			}
			}

			if (currentToken().type == Lumina::Token::Type::Operator)
			{
				result->elements.push_back(parseOperatorExpressionInstruction());
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