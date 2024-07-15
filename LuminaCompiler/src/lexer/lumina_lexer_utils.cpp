#include "lumina_lexer.hpp"

#include <fstream>
#include <iomanip>

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

	std::string to_string(const Lumina::Lexer::Instruction::Type& p_type)
	{
		switch (p_type)
		{
		case Lexer::Instruction::Type::Import:
			return ("Import");
		case Lexer::Instruction::Type::PipelineFlow:
			return ("PipelineFlow");
		case Lexer::Instruction::Type::PipelineDefinition:
			return ("PipelineDefinition");
		case Lexer::Instruction::Type::Structure:
			return ("Structure");
		case Lexer::Instruction::Type::Attribute:
			return ("Attribute");
		case Lexer::Instruction::Type::Constant:
			return ("Constant");
		case Lexer::Instruction::Type::Body:
			return ("Body");
		case Lexer::Instruction::Type::Expression:
			return ("Expression");
		case Lexer::Instruction::Type::CallParameters:
			return ("CallParameters");
		case Lexer::Instruction::Type::Texture:
			return ("Texture");
		case Lexer::Instruction::Type::Symbol:
			return ("Symbol");
		case Lexer::Instruction::Type::SymbolParameters:
			return ("SymbolParameters");
		case Lexer::Instruction::Type::Namespace:
			return ("Namespace");
		case Lexer::Instruction::Type::IfStatement:
			return ("IfStatement");
		case Lexer::Instruction::Type::WhileStatement:
			return ("WhileStatement");
		case Lexer::Instruction::Type::ForStatement:
			return ("ForStatement");
		case Lexer::Instruction::Type::Return:
			return ("Return");
		case Lexer::Instruction::Type::Discard:
			return ("Discard");
		case Lexer::Instruction::Type::Instruction:
			return ("Instruction");
		case Lexer::Instruction::Type::VariableDeclaration:
			return ("VariableDeclaration");
		}
		return ("Undefined type");
	}

	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction::Type& p_type)
	{
		p_os << to_string(p_type);
		return (p_os);
	}

	void Lexer::Instruction::print(std::fstream& p_outputStream, size_t p_tabulation, size_t p_startingLine) const
	{
		std::string tabulation = std::string(p_tabulation, ' ');

		size_t currentLine = p_startingLine;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].type == Tokenizer::Token::Type::MetaToken)
			{
				nestedInstructions[tokens[i].line].print(p_outputStream, p_tabulation + (nestedInstructions.size() != tokens.size() ? 4 : 0), currentLine);
			}
			else
			{
				if (currentLine != tokens[i].line)
				{
					currentLine = tokens[i].line;
					p_outputStream << std::endl << tabulation;
				}

				p_outputStream << tokens[i].content << " ";
			}
		}
	}

	void Lexer::Instruction::print(size_t p_tabulation, size_t p_startingLine) const
	{
		std::string tabulation = std::string(p_tabulation, ' ');

		size_t currentLine = p_startingLine;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].type == Tokenizer::Token::Type::MetaToken)
			{
				nestedInstructions[tokens[i].line].print(p_tabulation + (nestedInstructions.size() != tokens.size() ? 4 : 0), currentLine);
			}
			else
			{
				if (currentLine != tokens[i].line)
				{
					currentLine = tokens[i].line;
					std::cout << std::endl << tabulation;
				}

				std::cout << tokens[i].content << " ";
			}
		}
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
			_result.errors.push_back(CompilationError(p_error, _tokens.back().line, _tokens.back().fullLine, _tokens.back().column, _tokens.back().content.size()));
		}
		else
		{
			if (currentToken().type != TokenType::StringLitterals)
				_result.errors.push_back(CompilationError(p_error, currentToken().line, currentToken().fullLine, currentToken().column, currentToken().content.size()));
			else
			{
				size_t stringLenght = 1;
				while (currentToken().content[stringLenght] != '\n' && currentToken().content[stringLenght] != '\"')
					stringLenght++;
				if (currentToken().content[stringLenght] != '\"')
					stringLenght++;
				_result.errors.push_back(CompilationError(p_error, currentToken().line, currentToken().fullLine, currentToken().column, stringLenght));
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
			return ;

		size_t line = currentToken().line;
		while (hasTokenLeft() == true && currentToken().line == line)
			skipToken();
	}

	void Lexer::consume(Lexer::Instruction& p_instruction)
	{	
		p_instruction.insert(_tokens[_index]);
		_index++;
	}

	void Lexer::consume(Lexer::Instruction& p_instruction, Lexer::TokenType p_expectedTokenType)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType)
		{
			throw std::runtime_error("Unexpected token type found");
		}
		consume(p_instruction);
	}

	void Lexer::consume(Lexer::Instruction& p_instruction, Lexer::TokenType p_expectedTokenType, const std::string& p_errorMessage)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType)
		{
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}

	void Lexer::consume(Instruction& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, const std::string& p_errorMessage)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType ||
			currentToken().content != p_expectedTokenContent)
		{
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}

	void Lexer::consumeMultiple(Instruction& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, const std::string& p_errorMessage)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType)
		{
			throw std::runtime_error(p_errorMessage);
		}

		bool found = false;
		for (size_t i = 0; found == false && i < p_expectedTokenContents.size(); i++)
		{
			if (currentToken().content == p_expectedTokenContents[i])
			{
				found = true;
			}
		}

		if (found == false)
		{
			std::string errorMessage = "Invalid token value. Expected : ";
			for (size_t i = 0; i < p_expectedTokenContents.size(); i++)
			{
				if (i != 0)
					errorMessage += " - ";
				errorMessage += p_expectedTokenContents[i];
			}

			throw std::runtime_error(errorMessage);
		}
		consume(p_instruction);
	}
}