#include "lumina_lexer.hpp"

#include <fstream>
#include <iomanip>

namespace Lumina
{
	void Lexer::Element::insert(const Tokenizer::Token& p_token)
	{
		tokens.push_back(p_token);
	}

	void Lexer::Element::insertNestedElement(const Element& p_instruction)
	{
		tokens.push_back(Tokenizer::Token{ Tokenizer::Token::Type::MetaToken, "", nestedElement.size(), 0, ""});

		nestedElement.push_back(p_instruction);
	}

	std::ostream& operator << (std::ostream& p_os, const Lexer::Element::Type& p_type)
	{
		p_os << to_string(p_type);
		return (p_os);
	}

	void Lexer::Element::print(std::fstream& p_outputStream, size_t p_tabulation, size_t p_startingLine) const
	{
		std::string tabulation = std::string(p_tabulation, ' ');

		size_t currentLine = p_startingLine;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].type == Tokenizer::Token::Type::MetaToken)
			{
				nestedElement[tokens[i].line].print(p_outputStream, p_tabulation + (nestedElement.size() != tokens.size() ? 4 : 0), currentLine);
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

	void Lexer::Element::print(size_t p_tabulation, size_t p_startingLine) const
	{
		std::string tabulation = std::string(p_tabulation, ' ');

		size_t currentLine = p_startingLine;

		for (size_t i = 0; i < tokens.size(); i++)
		{
			if (tokens[i].type == Tokenizer::Token::Type::MetaToken)
			{
				nestedElement[tokens[i].line].print(p_tabulation + (nestedElement.size() != tokens.size() ? 4 : 0), currentLine);
			}
			else
			{
				if (currentLine != tokens[i].line)
				{
					currentLine = tokens[i].line;
					std::cout << std::endl << tabulation;
				}

				std::cout << "(" << i << ")" << tokens[i].content << " ";
			}
		}
	}

	std::vector<Tokenizer::Token> Lexer::Element::tokenList() const
	{
		std::vector<Tokenizer::Token> result;

		for (const auto& token : tokens)
		{
			if (token.type == TokenType::MetaToken)
			{
				std::vector<Tokenizer::Token> nestedTokens = nestedElement[token.line].tokenList();
			
				result.insert(result.end(), nestedTokens.begin(), nestedTokens.end());
			}
			else
			{
				result.push_back(token);
			}
		}

		return (result);
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
			return ;

		size_t line = currentToken().line;
		while (hasTokenLeft() == true && currentToken().line == line)
			skipToken();
	}

	void Lexer::consume(Lexer::Element& p_instruction)
	{	
		p_instruction.insert(_tokens[_index]);
		_index++;
	}

	void Lexer::consume(Lexer::Element& p_instruction, Lexer::TokenType p_expectedTokenType)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType)
		{
			throw std::runtime_error("Unexpected token type found");
		}
		consume(p_instruction);
	}

	void Lexer::consume(Lexer::Element& p_instruction, Lexer::TokenType p_expectedTokenType, const std::string& p_errorMessage)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType)
		{
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}

	void Lexer::consume(Element& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, const std::string& p_errorMessage)
	{
		if (hasTokenLeft() == false ||
			currentToken().type != p_expectedTokenType ||
			currentToken().content != p_expectedTokenContent)
		{
			throw std::runtime_error(p_errorMessage);
		}
		consume(p_instruction);
	}

	void Lexer::consumeMultiple(Element& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, const std::string& p_errorMessage)
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



	void Lexer::consumeAndReassign(Element& p_instruction, TokenType p_reassignedTokenType)
	{
		consume(p_instruction);
		_tokens.back().type = p_reassignedTokenType;
	}

	void Lexer::consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, TokenType p_reassignedTokenType)
	{
		consume(p_instruction, p_expectedTokenType);
		_tokens.back().type = p_reassignedTokenType;
	}

	void Lexer::consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, TokenType p_reassignedTokenType, const std::string& p_errorMessage)
	{
		consume(p_instruction, p_expectedTokenType, p_errorMessage);
		p_instruction.tokens.back().type = p_reassignedTokenType;
	}

	void Lexer::consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, TokenType p_reassignedTokenType, const std::string& p_errorMessage)
	{
		consume(p_instruction, p_expectedTokenType, p_expectedTokenContent, p_errorMessage);
		_tokens.back().type = p_reassignedTokenType;
	}

	void Lexer::consumeMultipleAndReassign(Element& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, TokenType p_reassignedTokenType, const std::string& p_errorMessage)
	{
		consumeMultiple(p_instruction, p_expectedTokenType, p_expectedTokenContents, p_errorMessage);
		_tokens.back().type = p_reassignedTokenType;
	}
}

std::string to_string(const Lumina::Lexer::Element::Type& p_type)
{
	switch (p_type)
	{
	case Lumina::Lexer::Element::Type::Import:
		return ("Import");
	case Lumina::Lexer::Element::Type::PipelineFlow:
		return ("PipelineFlow");
	case Lumina::Lexer::Element::Type::PipelineDefinition:
		return ("PipelineDefinition");
	case Lumina::Lexer::Element::Type::Structure:
		return ("Structure");
	case Lumina::Lexer::Element::Type::Attribute:
		return ("Attribute");
	case Lumina::Lexer::Element::Type::Constant:
		return ("Constant");
	case Lumina::Lexer::Element::Type::Body:
		return ("Body");
	case Lumina::Lexer::Element::Type::Expression:
		return ("Expression");
	case Lumina::Lexer::Element::Type::CallParameters:
		return ("CallParameters");
	case Lumina::Lexer::Element::Type::Texture:
		return ("Texture");
	case Lumina::Lexer::Element::Type::Symbol:
		return ("Symbol");
	case Lumina::Lexer::Element::Type::SymbolParameters:
		return ("SymbolParameters");
	case Lumina::Lexer::Element::Type::Namespace:
		return ("Namespace");
	case Lumina::Lexer::Element::Type::IfStatement:
		return ("IfStatement");
	case Lumina::Lexer::Element::Type::WhileStatement:
		return ("WhileStatement");
	case Lumina::Lexer::Element::Type::ForStatement:
		return ("ForStatement");
	case Lumina::Lexer::Element::Type::Return:
		return ("Return");
	case Lumina::Lexer::Element::Type::Discard:
		return ("Discard");
	case Lumina::Lexer::Element::Type::Element:
		return ("Element");
	case Lumina::Lexer::Element::Type::VariableDeclaration:
		return ("VariableDeclaration");
	case Lumina::Lexer::Element::Type::VariableAssignation:
		return ("VariableAssignation");
	}
	return ("Undefined type");
}