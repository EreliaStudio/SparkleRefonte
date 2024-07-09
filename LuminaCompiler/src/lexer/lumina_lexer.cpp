#include "lumina_lexer.hpp"

#define DEBUG_LINE() std::cout << __FUNCTION__ << "::" << __LINE__ << std::endl

namespace Lumina
{
	void Lexer::skipComment()
	{
		if (currentToken().type == TokenType::SingleLineComment)
		{
			size_t currentLine = currentToken().line;
			while (currentToken().line == currentLine)
				skipToken();
		}
		else if (currentToken().type == TokenType::MultiLineCommentStart)
		{
			while (currentToken().type != TokenType::MultiLineCommentStop)
			{
				skipToken();
			}
			skipToken();
		}
	}

	Lexer::Instruction Lexer::parsePipelineFlow()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Include;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "Input" }, "Expected pipeline flow");
		consume(result, TokenType::PipelineSeparator, "Expected a token \"->\"");
		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Expected pipeline flow");
		consume(result, TokenType::Separator, "Expected a token \":\"");
		consume(result, TokenType::Identifier, "Unexpected token found");
		consume(result, TokenType::Identifier, "Unexpected token found");
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parsePipelineDefinition()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Include;

		consume(result, TokenType::PipelineFlow, {"VertexPass", "FragmentPass"}, "Expected pipeline flow");
		consume(result, TokenType::OpenParenthesis, "Expected a token \"(\"");
		consume(result, TokenType::ClosedParenthesis, "Expected a token \")\"");
		consume(result, TokenType::OpenCurlyBracket, "Expectedt body opener token \"{\"");
		size_t nbCurly = 1;
		while (currentToken().type != TokenType::CloseCurlyBracket && nbCurly != 0)
		{
			std::cout << __FUNCTION__ << "::" << __LINE__ << " - " << currentToken() << std::endl;
			if (currentToken().type == TokenType::OpenCurlyBracket)
			{
				nbCurly++;
			}
			consume(result);
			if (currentToken().type == TokenType::CloseCurlyBracket)
			{
				nbCurly--;
			}
		}
		consume(result, TokenType::CloseCurlyBracket, "Expectedt body closer token \"}\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseInclude()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Include;

		consume(result, TokenType::Include);

		if (currentToken().content == "<")
		{
			size_t index = 0;

			consume(result, TokenType::Operator, "<", "Unexpected token found");
			while (currentToken().content != ">")
			{
				if (index != 0)
				{
					consume(result, TokenType::Operator, "/", "Unexpected token found");
				}
				consume(result, TokenType::Identifier, "Unexpected token found");
				if (currentToken().type == TokenType::Accessor)
				{
					consume(result, TokenType::Accessor, "Unexpected token found");
					consume(result, TokenType::Identifier, "Unexpected token found");
				}
				index++;
			}
			consume(result, TokenType::Operator, ">", "Unexpected token found");
		}
		else if (currentToken().type == TokenType::DoubleQuote)
		{
			size_t index = 0;
			
			consume(result, TokenType::DoubleQuote, "Unexpected token found");
			while (currentToken().type != TokenType::DoubleQuote)
			{
				if (index != 0)
				{
					consume(result, TokenType::Operator, "/", "Unexpected token found");
				}
				consume(result, TokenType::Identifier, "Unexpected token found");
				if (currentToken().type == TokenType::Accessor)
				{
					consume(result, TokenType::Accessor, "Unexpected token found");
					consume(result, TokenType::Identifier, "Unexpected token found");
				}
				index++;
			}
			consume(result, TokenType::DoubleQuote, "Unexpected token found");
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseStructure()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;



		return (result);
	}

	Lexer::Result Lexer::execute(const std::vector<Tokenizer::Token>& p_tokens)
	{
		_result = Result();
		_tokens = p_tokens;
		_index = 0;

		while (hasTokenLeft() == true)
		{
			try
			{
				Instruction instruction;

				switch (currentToken().type)
				{
				case TokenType::SingleLineComment:
				case TokenType::MultiLineCommentStart:
				{
					skipComment();
					break;
				}
				case TokenType::Include:
				{
					instruction = parseInclude();
					break;
				}
				case TokenType::PipelineFlow:
				{
					if (nextToken().type == TokenType::PipelineSeparator)
						instruction = parsePipelineFlow();
					else
						instruction = parsePipelineDefinition();
					break;
				}
				case TokenType::Structure:
				{

					break;
				}
				default:
				{
					insertError("Unexpected token [" + to_string(currentToken().type) + "] found");
					throw std::runtime_error("Unexpected token found");
				}
				}
				_result.instructions.push_back(instruction);
			}
			catch (...)
			{
				skipLine();
			}
		}

		return (_result);
	}
}