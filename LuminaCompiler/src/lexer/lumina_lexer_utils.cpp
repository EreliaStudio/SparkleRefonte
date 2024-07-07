#include "lumina_lexer.hpp"

namespace Lumina
{
	void Lexer::Instruction::addNestedInstruction(const Lexer::Instruction& p_instruction)
	{
		_tokens.push_back(Tokenizer::Token::createMetaToken(nestedInstructions.size()));
		nestedInstructions.push_back(p_instruction);
	}

	bool Lexer::hasTokenLeft() const
	{
		return(_index < _tokens.size());
	}

	const Tokenizer::Token& Lexer::currentToken() const
	{
		return (_tokens[_index]);
	}

	const Tokenizer::Token& Lexer::previousToken()
	{
		if (_index == 0)
		{
			insertError("No previous token found");
			throw std::runtime_error("Unexpected error");
		}
		return (_tokens[_index - 1]);
	}

	const Tokenizer::Token& Lexer::nextToken()
	{
		if (_index >= _tokens.size())
		{
			insertError("Expected more tokens");
			throw std::runtime_error("Unexpected error");
		}
		return (_tokens[_index + 1]);
	}

	void Lexer::skipWord()
	{
		advance();
	}

	void Lexer::skipLine()
	{
		size_t currentLine = currentToken().line;

		while (_index < _tokens.size() && currentToken().line == currentLine)
		{
			advance();
		}
	}

	void Lexer::skipComment()
	{
		if (currentToken().type != Tokenizer::Token::Type::MultilineComment && currentToken().type != Tokenizer::Token::Type::SingleLineComment)
			return;

		if (currentToken().type == Tokenizer::Token::Type::SingleLineComment)
		{
			skipLine();
		}
		else
		{
			while (_index < _tokens.size() && currentToken().type != Tokenizer::Token::Type::EndOfMultilineComment)
			{
				advance();
			}
			advance();
		}
	}

	void Lexer::insertError(const std::string& p_errorMessage)
	{
		_result.errors.push_back(CompilationError(p_errorMessage, currentToken().line, currentToken().fullLine, currentToken().column, currentToken().content.size()));
	}

	void Lexer::consumeCheck(Tokenizer::Token::Type p_type, const std::string& p_errorMessage)
	{
		if (_index >= _tokens.size())
		{
			_result.errors.push_back(CompilationError("Unexpected error detected", _tokens.back().line + 1, "", 0, 0));
			throw std::runtime_error("Unexpected error");
		}

		if (currentToken().type != p_type)
		{
			insertError(p_errorMessage);
			throw std::runtime_error("Invalid grammar detected");
		}
	}

	void Lexer::onSameLineCheck(size_t p_line)
	{
		if (currentToken().line != p_line)
		{
			_result.errors.push_back(CompilationError("Unexpected new line", previousToken().line, previousToken().fullLine, previousToken().fullLine.size(), 1));
			throw std::runtime_error("Unexpected error detected");
		}
	}

	void Lexer::consume(Instruction& p_instruction)
	{
		p_instruction._tokens.push_back(currentToken());
		advance();
	}

	void Lexer::consumeOnSameLine(Instruction& p_instruction)
	{
		onSameLineCheck(previousToken().line);
		consume(p_instruction);
	}

	void Lexer::consume(Instruction& p_instruction, Tokenizer::Token::Type p_type, const std::string& p_errorMessage)
	{
		consumeCheck(p_type, p_errorMessage);
		consume(p_instruction);
	}

	void Lexer::consumeOnSameLine(Instruction& p_instruction, Tokenizer::Token::Type p_type, const std::string& p_errorMessage)
	{
		consumeCheck(p_type, p_errorMessage);
		onSameLineCheck(previousToken().line);

		consume(p_instruction);
	}

	void Lexer::consumeAndCheckValue(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::string p_expectedValue, const std::string& p_errorMessage)
	{
		consumeAndCheckValues(p_instruction, p_type, {p_expectedValue}, p_errorMessage);
	}

	void Lexer::consumeOnSameLineAndCheckValue(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::string p_expectedValue, const std::string& p_errorMessage)
	{
		onSameLineCheck(previousToken().line);
		consumeAndCheckValue(p_instruction, p_type, p_expectedValue, p_errorMessage);
	}

	void Lexer::consumeAndCheckValues(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::vector<std::string> p_possibleValues, const std::string& p_errorMessage)
	{
		consumeCheck(p_type, p_errorMessage);

		bool found = false;
		for (const auto& tmp : p_possibleValues)
		{
			if (tmp == currentToken().content)
				found = true;
		}

		if (found == false)
		{
			std::string invalidValueMessage = "Unexpected value. Accepted values : ";
			for (const auto& tmp : p_possibleValues)
			{
				invalidValueMessage += "[" + tmp + "]";
			}
			insertError(invalidValueMessage);
			skipLine();
			throw std::runtime_error("Invalid grammar detected");
		}

		consume(p_instruction);
	}

	void Lexer::consumeOnSameLineAndCheckValues(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::vector<std::string> p_possibleValues, const std::string& p_errorMessage)
	{
		onSameLineCheck(previousToken().line);
		consumeAndCheckValues(p_instruction, p_type, p_possibleValues, p_errorMessage);
	}

	void Lexer::advance()
	{
		_index++;
	}

	void Lexer::appendInstruction(const Lexer::Instruction& p_instruction)
	{
		_result.instructions.push_back(p_instruction);
	}
}