#include "lumina_lexer.hpp"

namespace Lumina
{
	void Lexer::Instruction::insert(const Tokenizer::Token& p_token)
	{
		tokens.push_back(p_token);
	}

	void Lexer::Instruction::insertNestedInstruction(const Instruction& p_instruction)
	{
		tokens.push_back(Tokenizer::Token{ Tokenizer::Token::Type::MetaToken, "", nestedInstructions.size(), 0, ""});

		nestedInstructions.push_back(p_instruction);
	}

	Lexer::Lexer() :
		_index(0)
	{

	}

	void Lexer::setTokenToIndex(size_t p_index)
	{
		_index = p_index;
	}

	bool Lexer::hasTokenLeft(size_t p_offset) const
	{
		if (_index + p_offset >= _tokens.size())
		{
			return (false);
		}
		return (true);
	}

	const Tokenizer::Token& Lexer::previousToken()
	{
		return (_tokens[_index - 1]);
	}

	const Tokenizer::Token& Lexer::currentToken() const
	{
		return (_tokens[_index]);
	}

	const Tokenizer::Token& Lexer::nextToken()
	{
		return (_tokens[_index + 1]);
	}

	const Tokenizer::Token& Lexer::tokenAtIndex(size_t index) const
	{
		return (_tokens[_index + index]);
	}

	Lexer::Result Lexer::checkGrammar(const std::vector<Tokenizer::Token>& p_tokens)
	{
		return (Lexer().execute(p_tokens));
	}

	void Lexer::insertError(const std::string& p_error)
	{
		_result._errors.push_back(CompilationError(p_error, currentToken().line, currentToken().fullLine, currentToken().column, currentToken().content.size());
	}

	void Lexer::consume(Lexer::Instruction& p_instruction)
	{	
		p_instruction.insert(_tokens[_index]);
		_index++;
	}

	void Lexer::consume(Lexer::Instruction& p_instruction, Lexer::TokenType p_expectedTokenType)
	{
		if (currentToken().type != p_expectedTokenType)
		{
			insertError("Bad token found");
			throw std::runtime_error("Bad token found");
		}
		consume(p_instruction);
	}

	void Lexer::consume(Lexer::Instruction& p_instruction, Lexer::TokenType p_expectedTokenType, const std::string& p_errorMessage)
	{
		if (currentToken().type != p_expectedTokenType)
		{
			insertError(p_errorMessage);
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}
}