#include "lumina_lexer.hpp"

#include <fstream>
#include <iomanip>

namespace Lumina
{
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
		size_t firstIndex = _index;
		_index++;

		skipComment();

		size_t offset = _index - firstIndex;
		_index = firstIndex;
		return (_tokens[_index + offset]);
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
		if (hasTokenLeft() == false)
		{
			_result.errors.push_back(CompilationError(p_error, _tokens.back().fileName, _tokens.back().line, _tokens.back().fullLine, _tokens.back().column, _tokens.back().content.size()));
		}
		else
		{
			if (currentToken().type != TokenType::StringLitterals)
				_result.errors.push_back(CompilationError(p_error, currentToken().fileName, currentToken().line, currentToken().fullLine, currentToken().column, currentToken().content.size()));
			else
			{
				size_t stringLenght = 1;
				while (currentToken().content[stringLenght] != '\n' && currentToken().content[stringLenght] != '\"')
					stringLenght++;
				if (currentToken().content[stringLenght] != '\"')
					stringLenght++;
				_result.errors.push_back(CompilationError(p_error, currentToken().fileName, currentToken().line, currentToken().fullLine, currentToken().column, stringLenght));
			}
		}
	}

	void Lexer::skipToken()
	{
		_index++;
	}

	void Lexer::skipLine()
	{
		if (hasTokenLeft() == false)
			return;

		size_t line = currentToken().line;
		while (hasTokenLeft() == true && currentToken().line == line)
			skipToken();
	}

	void Lexer::skipComment()
	{
		if (hasTokenLeft() == false)
			return;

		if (currentToken().type == TokenType::SingleLineComment)
		{
			size_t currentLine = currentToken().line;
			while (hasTokenLeft() == true && currentToken().line == currentLine)
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
}