#include "lumina_lexer.hpp"

namespace Lumina
{
	std::shared_ptr<VariableDeclarationInstruction> Lexer::parseVariableDeclarationInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<VariableDeclarationInstruction> result = std::make_shared<VariableDeclarationInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			expect(Lumina::Token::Type::Assignator, "Expected an assignator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ReturnInstruction> Lexer::parseReturnInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<ReturnInstruction> result = std::make_shared<ReturnInstruction>();

		expect(Lumina::Token::Type::Return, "Expected a return token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->argument = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<DiscardInstruction> Lexer::parseDiscardInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<DiscardInstruction> result = std::make_shared<DiscardInstruction>();

		expect(Lumina::Token::Type::Discard, "Expected a discard token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<VariableDesignationInstruction> Lexer::parseVariableDesignationInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<VariableDesignationInstruction> result = std::make_shared<VariableDesignationInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::Accessor)
		{
			expect(Lumina::Token::Type::Accessor, "Expected an accessor token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<VariableAssignationInstruction> Lexer::parseVariableAssignationInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<VariableAssignationInstruction> result = std::make_shared<VariableAssignationInstruction>();

		result->name = parseVariableDesignationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::Assignator, "Expected an assignator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolBodyInstruction> Lexer::parseSymbolBodyInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<SymbolBodyInstruction> result = std::make_shared<SymbolBodyInstruction>();

		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				switch (currentToken().type)
				{
				case Lumina::Token::Type::Comment:
					skipToken();
					break;
				case Lumina::Token::Type::Identifier:
				case Lumina::Token::Type::NamespaceSeparator:
					if (describeVariableDeclarationInstruction() == true)
					{
						result->elements.push_back(parseVariableDeclarationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					}
					else if (describeSymbolCallInstruction() == true)
					{
						result->elements.push_back(parseSymbolCallInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
						expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
					}
					else if (describeVariableAssignationInstruction() == true)
					{
						result->elements.push_back(parseVariableAssignationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					}
					else
					{
						throw Lumina::TokenBasedError(_file, "Unexpected token type: " + to_string(currentToken().type) + COMPOSED_DEBUG_INFORMATION(p_debugInformation), currentToken());
					}
					break;
				case Lumina::Token::Type::Return:
					result->elements.push_back(parseReturnInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::Discard:
					result->elements.push_back(parseDiscardInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::IfStatement:
					result->elements.push_back(parseIfStatementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::WhileStatement:
					result->elements.push_back(parseWhileLoopInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::ForStatement:
					result->elements.push_back(parseForLoopInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				default:
					throw Lumina::TokenBasedError(_file, "Unexpected token type: " + to_string(currentToken().type) + COMPOSED_DEBUG_INFORMATION(p_debugInformation), currentToken());
				}
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);
				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolInstruction> Lexer::parseSymbolInstruction(const std::string& p_debugInformation)
	{
		std::shared_ptr<SymbolInstruction> result = std::make_shared<SymbolInstruction>();

		result->returnType = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			result->parameters.push_back(parseSymbolParameterInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			if (currentToken().type != Lumina::Token::Type::CloseParenthesis)
				expect(Lumina::Token::Type::Comma, "Expected a comma." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}
}