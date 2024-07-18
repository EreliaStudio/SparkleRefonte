#include "lumina_lexer.hpp"

#include "lumina_utils.hpp"

#include <iomanip>

namespace Lumina
{
	void Lexer::skipComment()
	{
		if (hasTokenLeft() == false)
			return ;

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

	Lexer::Element Lexer::parsePipelineFlow()
	{
		Lexer::Element result;

		result.type = Element::Type::PipelineFlow;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "Input" }, "Entry pipeflow can only be Input toward VertexPass, or VertexPass toward FragmentPass" + DEBUG_INFORMATION);
		consume(result, TokenType::PipelineSeparator, "Expected a token \"->\"" + DEBUG_INFORMATION);
		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Output pipeflow can only be VertexPass when coming from Input, or FragmentPass when coming from VertexPass" + DEBUG_INFORMATION);
		consume(result, TokenType::Separator, "Expected a token \":\"" + DEBUG_INFORMATION);
		consumeAndReassign(result, TokenType::Identifier, TokenType::Type, "Unexpected token found" + DEBUG_INFORMATION);
		consumeAndReassign(result, TokenType::Identifier, TokenType::Name, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parsePipelineDefinition()
	{
		Lexer::Element result;

		result.type = Element::Type::PipelineDefinition;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Expected a valid pipeline pass definition" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a token \"(\"" + DEBUG_INFORMATION);
		consume(result, TokenType::ClosedParenthesis, "Expected a token \")\"" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		result.insertNestedElement(parseSymbolBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseImport()
	{
		Lexer::Element result;

		result.type = Element::Type::Import;

		consume(result, TokenType::Import, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::StringLitterals, "Only \'\"\' is concidered as a valid import string litteral delimitor" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseCallParameters()
	{
		Element result;

		result.type = Element::Type::CallParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			try
			{
				result.insertNestedElement(parseExpression());
				if (currentToken().type != TokenType::ClosedParenthesis)
					consume(result, TokenType::Comma, "Expected parameter separator \",\"" + DEBUG_INFORMATION);
			}
			catch (...)
			{
				while (currentToken().type != TokenType::ClosedParenthesis)
					skipToken();
			}
		}

		return (result);
	}

	void Lexer::expendExpression(Lexer::Element& p_instruction)
	{
		switch (currentToken().type)
		{
		case TokenType::Number:
		{
			consume(p_instruction, TokenType::Number, "Unexpected token found" + DEBUG_INFORMATION);
			break;
		}
		case TokenType::Bool:
		{
			consume(p_instruction, TokenType::Bool, "Unexpected token found" + DEBUG_INFORMATION);
			break;
		}
		case TokenType::StringLitterals:
		{
			consume(p_instruction, TokenType::StringLitterals, "Unexpected token found" + DEBUG_INFORMATION);
			break;
		}
		case TokenType::NamespaceSeparator:
		{
			consume(p_instruction, TokenType::NamespaceSeparator, "Unexpected token found" + DEBUG_INFORMATION);
		}
		case TokenType::Identifier:
		{
			size_t startIdentifierIndex = p_instruction.tokens.size();

			expendTypeIdentifier(p_instruction, DEBUG_INFORMATION);

			if (currentToken().type == TokenType::Accessor)
			{
				p_instruction.tokens.back().type = TokenType::Name;
				while (currentToken().type == TokenType::Accessor)
				{
					consume(p_instruction, TokenType::Accessor, "Expected an accessor token \".\"" + DEBUG_INFORMATION);
					consume(p_instruction, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
				}
			}
			
			if (currentToken().type == TokenType::OpenParenthesis)
			{
				for (size_t i = startIdentifierIndex; i < p_instruction.tokens.size(); i++)
				{
					if (p_instruction.tokens[i].type == TokenType::Identifier)
						p_instruction.tokens[i].type = TokenType::SymbolCall;
				}
				consume(p_instruction, TokenType::OpenParenthesis, "Expected opened parenthesis token \"(\"" + DEBUG_INFORMATION);
				p_instruction.insertNestedElement(parseCallParameters());
				consume(p_instruction, TokenType::ClosedParenthesis, "Expected closed parenthesis token \")\"" + DEBUG_INFORMATION);
			}
			else
			{
				for (size_t i = startIdentifierIndex; i < p_instruction.tokens.size(); i++)
				{
					if (p_instruction.tokens[i].type == TokenType::Identifier)
						p_instruction.tokens[i].type = TokenType::Name;
				}
			}

			if (currentToken().type == TokenType::Accessor)
			{
				p_instruction.tokens.back().type = TokenType::Name;
				while (currentToken().type == TokenType::Accessor)
				{
					consume(p_instruction, TokenType::Accessor, "Expected an accessor token \".\"" + DEBUG_INFORMATION);
					consumeAndReassign(p_instruction, TokenType::Identifier, TokenType::Name, "Unexpected token found" + DEBUG_INFORMATION);
				}
			}
			break;
		}
		}
	}

	void Lexer::expendTypeIdentifier(Lexer::Element& p_instruction, const std::string& debugInformation)
	{
		consume(p_instruction, TokenType::Identifier, "Unexpected token found" + debugInformation);
		while (currentToken().type == TokenType::NamespaceSeparator)
		{
			consume(p_instruction, TokenType::NamespaceSeparator, "Expected an accessor token \"::\"" + debugInformation);
			consume(p_instruction, TokenType::Identifier, "Unexpected token found" + debugInformation);
		}
	}

	Lexer::Element Lexer::parseExpression()
	{
		Lexer::Element result;

		result.type = Element::Type::Expression;

		skipComment();
		if (currentToken().type == TokenType::OpenParenthesis)
		{
			consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
			skipComment();
			result.insertNestedElement(parseExpression());
			skipComment();
			consume(result, TokenType::ClosedParenthesis, "Expected a \")\"" + DEBUG_INFORMATION);
			skipComment();
		}
		else
		{
			skipComment();
			expendExpression(result);
			skipComment();
		}

		skipComment();
		while (currentToken().type == TokenType::Operator)
		{
			consume(result, TokenType::Operator, "Unexpected token found" + DEBUG_INFORMATION);
			if (previousToken().content == "--" || previousToken().content == "++")
				break;
			skipComment();
			expendExpression(result);
			skipComment();
		}

		return (result);
	}

	Lexer::Element Lexer::parseBlockElement()
	{
		Lexer::Element result;

		result.type = Element::Type::BlockElement;

		skipComment();
		expendTypeIdentifier(result, DEBUG_INFORMATION);

		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);
		skipComment();

		return (result);
	}

	Lexer::Element Lexer::parseBlockBody()
	{
		Lexer::Element result;

		result.type = Element::Type::Body;

		while (currentToken().type != TokenType::ClosedCurlyBracket)
		{
			try
			{
				result.insertNestedElement(parseBlockElement());
			}
			catch (std::runtime_error& e)
			{
				insertError(e.what());
				skipLine();
			}
		}

		return (result);
	}

	Lexer::Element Lexer::parseStructureBlock()
	{
		Lexer::Element result;

		result.type = Element::Type::Structure;

		consume(result, TokenType::Structure, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		result.insertNestedElement(parseBlockBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseAttributeBlock()
	{
		Lexer::Element result;

		result.type = Element::Type::Attribute;

		consume(result, TokenType::AttributeBlock, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		try
		{
			result.insertNestedElement(parseBlockBody());
		}
		catch (...)
		{
			skipLine();
		}
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseConstantBlock()
	{
		Lexer::Element result;

		result.type = Element::Type::Constant;

		consume(result, TokenType::ConstantBlock, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		try
		{
			result.insertNestedElement(parseBlockBody());
		}
		catch (...)
		{
			skipLine();
		}
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseTexture()
	{
		Lexer::Element result;

		result.type = Element::Type::Texture;

		consume(result, TokenType::Texture, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseSymbolParameters()
	{
		Lexer::Element result;

		result.type = Element::Type::SymbolParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			try
			{
				skipComment();
				expendTypeIdentifier(result, DEBUG_INFORMATION);
				skipComment();
				consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
				skipComment();
				if (currentToken().type != TokenType::ClosedParenthesis)
					consume(result, TokenType::Comma, "Expected a token \",\"" + DEBUG_INFORMATION);
			}
			catch (...)
			{
				while (currentToken().type != TokenType::ClosedParenthesis)
					skipToken();
			}
		}

		return (result);
	}

	Lexer::Element Lexer::parseFunctionInstruction()
	{
		Lexer::Element result;
		result.type = Element::Type::Element;

		skipComment();
		if (currentToken().type == TokenType::Identifier && nextToken().type == TokenType::Identifier)
		{
			result.type = Element::Type::VariableDeclaration;

			expendTypeIdentifier(result, DEBUG_INFORMATION);
			skipComment();
			consumeAndReassign(result, TokenType::Identifier, TokenType::Name, "Expected a variable name" + DEBUG_INFORMATION);

			skipComment();
			if (currentToken().type == TokenType::Assignator)
			{
				consume(result, TokenType::Assignator, "Expected an assignator token \"=\"" + DEBUG_INFORMATION);
				result.insertNestedElement(parseExpression());
			}
		}
		else
		{
			result = parseExpression();

			if (currentToken().type == TokenType::Assignator)
			{
				result.type = Element::Type::VariableAssignation;
				consume(result, TokenType::Assignator, "Expected an assignator token \"=\"" + DEBUG_INFORMATION);
				result.insertNestedElement(parseExpression());
			}
		}

		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return result;
	}

	Lexer::Element Lexer::parseIfStatement()
	{
		Lexer::Element result;

		result.type = Element::Type::IfStatement;

		consume(result, TokenType::IfStatement, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
		skipComment();
		result.insertNestedElement(parseExpression());

		if (currentToken().type != TokenType::ClosedParenthesis)
		{
			insertError("Invalid statement condition syntax : missing a \")\"" + DEBUG_INFORMATION);
			throw std::runtime_error("Invalid statement condition syntax" + DEBUG_INFORMATION);
		}
		skipComment();

		consume(result, TokenType::ClosedParenthesis, "Expected a \")\"" + DEBUG_INFORMATION);

		skipComment();

		if (currentToken().type == TokenType::OpenCurlyBracket)
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
			result.insertNestedElement(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedElement(parseOnelinerSymbolBody());
		}

		return (result);
	}

	Lexer::Element Lexer::parseReturn()
	{
		Lexer::Element result;

		result.type = Element::Type::Return;

		consume(result, TokenType::Return, "Expected a 'return' keyword" + DEBUG_INFORMATION);

		skipComment();

		if (currentToken().type != TokenType::EndOfSentence)
		{
			result.insertNestedElement(parseExpression());
		}

		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Element Lexer::parseDiscard()
	{
		Lexer::Element result;
		result.type = Element::Type::Discard;

		consume(result, TokenType::Discard, "Expected a \"discard\" keyword" + DEBUG_INFORMATION);
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return result;
	}

	Lexer::Element Lexer::parseWhileStatement()
	{
		Lexer::Element result;

		result.type = Element::Type::WhileStatement;

		consume(result, TokenType::WhileStatement, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
		result.insertNestedElement(parseExpression());
		consume(result, TokenType::ClosedParenthesis, "Expected a \")\"" + DEBUG_INFORMATION);

		if (currentToken().type == TokenType::OpenCurlyBracket)
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
			result.insertNestedElement(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedElement(parseOnelinerSymbolBody());
		}

		return (result);
	}

	Lexer::Element Lexer::parseForStatement()
	{
		Lexer::Element result;
		result.type = Element::Type::ForStatement;

		consume(result, TokenType::ForStatement, "Expected a 'for' keyword" + DEBUG_INFORMATION);

		consume(result, TokenType::OpenParenthesis, "Expected a '('" + DEBUG_INFORMATION);

		result.insertNestedElement(parseFunctionInstruction());
		result.insertNestedElement(parseExpression());
		consume(result, TokenType::EndOfSentence, "Expected a ';' after condition" + DEBUG_INFORMATION);
		result.insertNestedElement(parseExpression());

		consume(result, TokenType::ClosedParenthesis, "Expected a ')'" + DEBUG_INFORMATION);

		if (currentToken().type == TokenType::OpenCurlyBracket)
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token '{'" + DEBUG_INFORMATION);
			result.insertNestedElement(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token '}'" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedElement(parseFunctionInstruction());
		}

		return result;
	}

	Lexer::Element Lexer::parseOnelinerSymbolBody()
	{
		Lexer::Element result;

		result.type = Element::Type::Body;

		do
		{
			switch (currentToken().type)
			{
			case TokenType::SingleLineComment:
			case TokenType::MultiLineCommentStart:
			{
				skipComment();
				break;
			}
			case TokenType::EndOfSentence:
			{

				break;
			}
			case TokenType::IfStatement:
			{
				result.insertNestedElement(parseIfStatement());
				break;
			}
			case TokenType::WhileStatement:
			{
				result.insertNestedElement(parseWhileStatement());
				break;
			}
			case TokenType::ForStatement:
			{
				result.insertNestedElement(parseForStatement());
				break;
			}
			case TokenType::Return:
			{
				result.insertNestedElement(parseReturn());
				break;
			}
			case TokenType::Discard:
			{
				result.insertNestedElement(parseDiscard());
				break;
			}
			case TokenType::OpenCurlyBracket:
			{
				consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
				result.insertNestedElement(parseSymbolBody());
				consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
				break;
			}
			default:
			{
				result.insertNestedElement(parseFunctionInstruction());
				break;
			}
			}

		} while (currentToken().type != TokenType::EndOfSentence);

		return (result);
	}

	Lexer::Element Lexer::parseSymbolBody()
	{
		Lexer::Element result;

		result.type = Element::Type::Body;

		skipComment();
		while (currentToken().type != TokenType::ClosedCurlyBracket)
		{
			try
			{
			switch (currentToken().type)
			{

			case TokenType::OpenCurlyBracket:
			{
				consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
				result.insertNestedElement(parseSymbolBody());
				consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
				break;
			}

			case TokenType::SingleLineComment:
			case TokenType::MultiLineCommentStart:
			{
				skipComment();
				break;
			}
			case TokenType::IfStatement:
			{
				result.insertNestedElement(parseIfStatement());
				break;
			}
			case TokenType::WhileStatement:
			{
				result.insertNestedElement(parseWhileStatement());
				break;
			}
			case TokenType::ForStatement:
			{
				result.insertNestedElement(parseForStatement());
				break;
			}
			case TokenType::Return:
			{
				result.insertNestedElement(parseReturn());
				break;
			}
			case TokenType::Discard:
			{
				result.insertNestedElement(parseDiscard());
				break;
			}
			default:
			{
				result.insertNestedElement(parseFunctionInstruction());
				break;
			}
			}

			}
			catch (...)
			{
				insertError("Invalid token detected [" + currentToken().content + "]");
				skipLine();
			}
		}

		return (result);
	}

	Lexer::Element Lexer::parseSymbol()
	{
		Lexer::Element result;

		result.type = Element::Type::Symbol;

		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		if (currentToken().type == TokenType::NamespaceSeparator)
		{
			while (currentToken().type == TokenType::NamespaceSeparator)
			{
				consume(result, TokenType::NamespaceSeparator, "Expected an accessor token \"::\"" + DEBUG_INFORMATION);
				consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
			}
		}
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenParenthesis, "Expected opened parenthesis token \"(\"" + DEBUG_INFORMATION);
		result.insertNestedElement(parseSymbolParameters());
		consume(result, TokenType::ClosedParenthesis, "Expected closed parenthesis token \")\"" + DEBUG_INFORMATION);
		skipComment();
		if (currentToken().type == TokenType::EndOfSentence)
		{
			consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);
		}
		else
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
			result.insertNestedElement(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}

		return (result);
	}

	Lexer::Element Lexer::parseNamespaceBody()
	{
		Lexer::Element result;

		result.type = Element::Type::Body;

		while (currentToken().type != TokenType::ClosedCurlyBracket)
		{
			Element instruction;

			switch (currentToken().type)
			{
			case TokenType::SingleLineComment:
			case TokenType::MultiLineCommentStart:
			{
				skipComment();
				break;
			}
			case TokenType::Structure:
			{
				instruction = parseStructureBlock();
				break;
			}
			case TokenType::AttributeBlock:
			{
				instruction = parseAttributeBlock();
				break;
			}
			case TokenType::ConstantBlock:
			{
				instruction = parseConstantBlock();
				break;
			}
			case TokenType::Texture:
			{
				instruction = parseTexture();
				break;
			}
			case TokenType::Identifier:
			{
				instruction = parseSymbol();
				break;
			}
			case TokenType::Namespace:
			{
				instruction = parseNamespace();
				break;
			}
			default:
			{
				insertError("Unexpected token [" + to_string(currentToken().type) + "] found" + DEBUG_INFORMATION);
				throw std::runtime_error("Unexpected token found" + DEBUG_INFORMATION);
			}
			}

			result.insertNestedElement(instruction);
		}

		return (result);
	}

	Lexer::Element Lexer::parseNamespace()
	{
		Lexer::Element result;

		result.type = Element::Type::Namespace;

		consume(result, TokenType::Namespace, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		result.insertNestedElement(parseNamespaceBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);

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
				Element instruction;

				switch (currentToken().type)
				{
				case TokenType::SingleLineComment:
				case TokenType::MultiLineCommentStart:
				{
					skipComment();
					break;
				}
				case TokenType::Import:
				{
					instruction = parseImport();
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
					instruction = parseStructureBlock();
					break;
				}
				case TokenType::AttributeBlock:
				{
					instruction = parseAttributeBlock();
					break;
				}
				case TokenType::ConstantBlock:
				{
					instruction = parseConstantBlock();
					break;
				}
				case TokenType::Texture:
				{
					instruction = parseTexture();
					break;
				}
				case TokenType::Identifier:
				{
					instruction = parseSymbol();
					break;
				}
				case TokenType::Namespace:
				{
					instruction = parseNamespace();
					break;
				}
				case TokenType::OpenCurlyBracket:
				{
					size_t nbCurlyBracket = 0;
					while (currentToken().type != TokenType::ClosedCurlyBracket || nbCurlyBracket > 1)
					{
						if (currentToken().type == TokenType::OpenCurlyBracket)
							nbCurlyBracket++;
						else if (currentToken().type == TokenType::ClosedCurlyBracket)
							nbCurlyBracket--;
						_index++;
					}
					_index++;
					break;
				}
				default:
				{
					throw std::runtime_error("Unexpected token [" + to_string(currentToken().type) + "] found" + DEBUG_INFORMATION);
				}
				}
				if (instruction.tokens.size() != 0)
				{
					_result.elements.push_back(instruction);
				}
			}
			catch (std::runtime_error& e)
			{
				insertError(e.what());
				skipLine();
			}
		}
		return (_result);
	}
}