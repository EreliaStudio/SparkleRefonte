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

	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction::Type& p_type)
	{
		switch (p_type)
		{
		case Lexer::Instruction::Type::Import:
			p_os << "Import"; break;
		case Lexer::Instruction::Type::PipelineFlow:
			p_os << "PipelineFlow"; break;
		case Lexer::Instruction::Type::PipelineDefinition:
			p_os << "PipelineDefinition"; break;
		case Lexer::Instruction::Type::Structure:
			p_os << "Structure"; break;
		case Lexer::Instruction::Type::Attribute:
			p_os << "Attribute"; break;
		case Lexer::Instruction::Type::Constant:
			p_os << "Constant"; break;
		case Lexer::Instruction::Type::Body:
			p_os << "Body"; break;
		case Lexer::Instruction::Type::Expression:
			p_os << "Expression"; break;
		case Lexer::Instruction::Type::CallParameters:
			p_os << "CallParameters"; break;
		case Lexer::Instruction::Type::Texture:
			p_os << "Texture"; break;
		case Lexer::Instruction::Type::Symbol:
			p_os << "Symbol"; break;
		case Lexer::Instruction::Type::SymbolParameters:
			p_os << "SymbolParameters"; break;
		case Lexer::Instruction::Type::Namespace:
			p_os << "Namespace"; break;
		case Lexer::Instruction::Type::IfStatement:
			p_os << "IfStatement"; break;
		case Lexer::Instruction::Type::WhileStatement:
			p_os << "WhileStatement"; break;
		case Lexer::Instruction::Type::ForStatement:
			p_os << "ForStatement"; break;
		case Lexer::Instruction::Type::Return:
			p_os << "Return"; break;
		case Lexer::Instruction::Type::Discard:
			p_os << "Discard"; break;
		case Lexer::Instruction::Type::Instruction:
			p_os << "Instruction"; break;
		case Lexer::Instruction::Type::VariableDeclaration:
			p_os << "VariableDeclaration"; break;
		default:
			p_os << "Undefined type name"; break;
		}
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
		if (_index >= _tokens.size())
		{
			throw std::runtime_error("Invalid token index");
		}
		
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

	void Lexer::consume(Instruction& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, const std::string& p_errorMessage)
	{
		if (currentToken().type != p_expectedTokenType || currentToken().content != p_expectedTokenContent)
		{
			insertError(p_errorMessage);
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}

	void Lexer::consumeMultiple(Instruction& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, const std::string& p_errorMessage)
	{
		if (currentToken().type != p_expectedTokenType)
		{
			insertError(p_errorMessage);
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
			insertError(errorMessage);
			throw std::runtime_error(errorMessage);
		}
		consume(p_instruction);
	}
}