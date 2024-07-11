#include "lumina_lexer.hpp"

#include <iomanip>

#ifndef NDEBUG
#define DEBUG_INFORMATION std::string(" ") + __FUNCTION__ + "::" + std::to_string(__LINE__)
#else
#define DEBUG_INFORMATION std::string()
#endif

namespace Lumina
{
	void Lexer::skipComment()
	{
		if (hasTokenLeft() == false)
			return ;

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

		result.type = Instruction::Type::PipelineFlow;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "Input" }, "Expected a valid pipeline flow" + DEBUG_INFORMATION);
		consume(result, TokenType::PipelineSeparator, "Expected a token \"->\"" + DEBUG_INFORMATION);
		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Expected a valid pipeline flow" + DEBUG_INFORMATION);
		consume(result, TokenType::Separator, "Expected a token \":\"" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Instruction Lexer::parsePipelineDefinition()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::PipelineDefinition;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Expected a valid pipeline pass definition" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a token \"(\"" + DEBUG_INFORMATION);
		consume(result, TokenType::ClosedParenthesis, "Expected a token \")\"" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		result.insertNestedInstruction(parseSymbolBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Instruction Lexer::parseImport()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Import;

		consume(result, TokenType::Import, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::StringLitterals, "Unexpected token found" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Instruction Lexer::parseCallParameters()
	{
		Instruction result;

		result.type = Instruction::Type::CallParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			try
			{
				result.insertNestedInstruction(parseExpression());
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

	void Lexer::expendExpression(Lexer::Instruction& p_instruction)
	{
		switch (currentToken().type)
		{
		case TokenType::Number:
		{
			consume(p_instruction, TokenType::Number, "Unexpected token found" + DEBUG_INFORMATION);
			break;
		}
		case TokenType::StringLitterals:
		{
			consume(p_instruction, TokenType::StringLitterals, "Unexpected token found" + DEBUG_INFORMATION);
			break;
		}
		case TokenType::Identifier:
		{
			consume(p_instruction, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
			if (currentToken().type == TokenType::NamespaceSeparator)
			{
				while (currentToken().type == TokenType::NamespaceSeparator)
				{
					consume(p_instruction, TokenType::NamespaceSeparator, "Expected an accessor token \".\"" + DEBUG_INFORMATION);
					consume(p_instruction, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
				}
			}
			if (currentToken().type == TokenType::Accessor)
			{
				while (currentToken().type == TokenType::Accessor)
				{
					consume(p_instruction, TokenType::Accessor, "Expected an accessor token \".\"" + DEBUG_INFORMATION);
					consume(p_instruction, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
				}
			}
			if (currentToken().type == TokenType::OpenParenthesis)
			{
				consume(p_instruction, TokenType::OpenParenthesis, "Expected opened parenthesis token \"(\"" + DEBUG_INFORMATION);
				p_instruction.insertNestedInstruction(parseCallParameters());
				consume(p_instruction, TokenType::ClosedParenthesis, "Expected closed parenthesis token \")\"" + DEBUG_INFORMATION);
			}

			if (currentToken().type == TokenType::Accessor)
			{
				while (currentToken().type == TokenType::Accessor)
				{
					consume(p_instruction, TokenType::Accessor, "Expected an accessor token \".\"" + DEBUG_INFORMATION);
					consume(p_instruction, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
				}
			}
			break;
		}
		}
	}

	Lexer::Instruction Lexer::parseExpression()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Expression;

		skipComment();
		if (currentToken().type == TokenType::OpenParenthesis)
		{
			consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
			skipComment();
			result.insertNestedInstruction(parseExpression());
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

	Lexer::Instruction Lexer::parseBlockBody(bool p_parseAssignator)
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

		while (currentToken().type != TokenType::ClosedCurlyBracket)
		{
			skipComment();
			consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
			skipComment();
			consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
			skipComment();
			if (p_parseAssignator == true && currentToken().type == TokenType::Assignator)
			{
				consume(result, TokenType::Assignator, "Expected an assignator token \"=\"" + DEBUG_INFORMATION);
				result.insertNestedInstruction(parseExpression());
			}
			consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);
			skipComment();
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseStructureBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::Structure, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		try
		{
			result.insertNestedInstruction(parseBlockBody(false));
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

	Lexer::Instruction Lexer::parseAttributeBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::AttributeBlock, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		try
		{
			result.insertNestedInstruction(parseBlockBody(true));
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

	Lexer::Instruction Lexer::parseConstantBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::ConstantBlock, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		try
		{
			result.insertNestedInstruction(parseBlockBody(true));
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

	Lexer::Instruction Lexer::parseTexture()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Texture;

		consume(result, TokenType::Texture, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Instruction Lexer::parseSymbolParameters()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::SymbolParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			try
			{
				skipComment();
				consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
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

	Lexer::Instruction Lexer::parseFunctionInstruction()
	{
		Lexer::Instruction result;
		result.type = Instruction::Type::Instruction;

		Lexer::Instruction expression;

		if (currentToken().type == TokenType::Identifier && nextToken().type == TokenType::Identifier)
		{
			expression.type = Instruction::Type::VariableDeclaration;

			consume(expression, TokenType::Identifier, "Expected a type identifier" + DEBUG_INFORMATION);
			skipComment();
			consume(expression, TokenType::Identifier, "Expected a variable name" + DEBUG_INFORMATION);

			if (currentToken().type == TokenType::Assignator)
			{
				consume(expression, TokenType::Assignator, "Expected an assignator token \"=\"" + DEBUG_INFORMATION);
				expression.insertNestedInstruction(parseExpression());
			}
		}
		else
		{
			expression = parseExpression();

			if (currentToken().type == TokenType::Assignator)
			{
				consume(expression, TokenType::Assignator, "Expected an assignator token \"=\"" + DEBUG_INFORMATION);
				expression.insertNestedInstruction(parseExpression());
			}
		}

		consume(expression, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);
		result.insertNestedInstruction(expression);

		return result;
	}

	Lexer::Instruction Lexer::parseIfStatement()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::IfStatement;

		consume(result, TokenType::IfStatement, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
		skipComment();
		result.insertNestedInstruction(parseExpression());

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
			result.insertNestedInstruction(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedInstruction(parseOnelinerSymbolBody());
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseReturn()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Return;

		consume(result, TokenType::Return, "Expected a 'return' keyword" + DEBUG_INFORMATION);

		skipComment();

		if (currentToken().type != TokenType::EndOfSentence)
		{
			result.insertNestedInstruction(parseExpression());
		}

		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return (result);
	}

	Lexer::Instruction Lexer::parseDiscard()
	{
		Lexer::Instruction result;
		result.type = Instruction::Type::Discard;

		consume(result, TokenType::Discard, "Expected a \"discard\" keyword" + DEBUG_INFORMATION);
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);

		return result;
	}

	Lexer::Instruction Lexer::parseWhileStatement()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::WhileStatement;

		consume(result, TokenType::WhileStatement, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenParenthesis, "Expected a \"(\"" + DEBUG_INFORMATION);
		result.insertNestedInstruction(parseExpression());
		consume(result, TokenType::ClosedParenthesis, "Expected a \")\"" + DEBUG_INFORMATION);

		if (currentToken().type == TokenType::OpenCurlyBracket)
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
			result.insertNestedInstruction(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedInstruction(parseOnelinerSymbolBody());
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseForStatement()
	{
		Lexer::Instruction result;
		result.type = Instruction::Type::ForStatement;

		consume(result, TokenType::ForStatement, "Expected a 'for' keyword" + DEBUG_INFORMATION);

		consume(result, TokenType::OpenParenthesis, "Expected a '('" + DEBUG_INFORMATION);

		result.insertNestedInstruction(parseFunctionInstruction());
		result.insertNestedInstruction(parseExpression());
		consume(result, TokenType::EndOfSentence, "Expected a ';' after condition" + DEBUG_INFORMATION);
		result.insertNestedInstruction(parseExpression());

		consume(result, TokenType::ClosedParenthesis, "Expected a ')'" + DEBUG_INFORMATION);

		if (currentToken().type == TokenType::OpenCurlyBracket)
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token '{'" + DEBUG_INFORMATION);
			result.insertNestedInstruction(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token '}'" + DEBUG_INFORMATION);
		}
		else
		{
			result.insertNestedInstruction(parseFunctionInstruction());
		}

		return result;
	}

	Lexer::Instruction Lexer::parseOnelinerSymbolBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

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
				result.insertNestedInstruction(parseIfStatement());
				break;
			}
			case TokenType::WhileStatement:
			{
				result.insertNestedInstruction(parseWhileStatement());
				break;
			}
			case TokenType::ForStatement:
			{
				result.insertNestedInstruction(parseForStatement());
				break;
			}
			case TokenType::Return:
			{
				result.insertNestedInstruction(parseReturn());
				break;
			}
			case TokenType::Discard:
			{
				result.insertNestedInstruction(parseDiscard());
				break;
			}
			case TokenType::OpenCurlyBracket:
			{
				consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
				result.insertNestedInstruction(parseSymbolBody());
				consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
				break;
			}
			default:
			{
				result.insertNestedInstruction(parseFunctionInstruction());
				break;
			}
			}

		} while (currentToken().type != TokenType::EndOfSentence);

		return (result);
	}

	Lexer::Instruction Lexer::parseSymbolBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

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
				result.insertNestedInstruction(parseSymbolBody());
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
				result.insertNestedInstruction(parseIfStatement());
				break;
			}
			case TokenType::WhileStatement:
			{
				result.insertNestedInstruction(parseWhileStatement());
				break;
			}
			case TokenType::ForStatement:
			{
				result.insertNestedInstruction(parseForStatement());
				break;
			}
			case TokenType::Return:
			{
				result.insertNestedInstruction(parseReturn());
				break;
			}
			case TokenType::Discard:
			{
				result.insertNestedInstruction(parseDiscard());
				break;
			}
			default:
			{
				result.insertNestedInstruction(parseFunctionInstruction());
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

	Lexer::Instruction Lexer::parseSymbol()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Symbol;

		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		skipComment();
		consume(result, TokenType::OpenParenthesis, "Expected opened parenthesis token \"(\"" + DEBUG_INFORMATION);
		result.insertNestedInstruction(parseSymbolParameters());
		consume(result, TokenType::ClosedParenthesis, "Expected closed parenthesis token \")\"" + DEBUG_INFORMATION);
		skipComment();
		if (currentToken().type == TokenType::EndOfSentence)
		{
			consume(result, TokenType::EndOfSentence, "Expected a token \";\"" + DEBUG_INFORMATION);
		}
		else
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
			result.insertNestedInstruction(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"" + DEBUG_INFORMATION);
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseNamespaceBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

		while (currentToken().type != TokenType::ClosedCurlyBracket)
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
			default:
			{
				insertError("Unexpected token [" + to_string(currentToken().type) + "] found" + DEBUG_INFORMATION);
				throw std::runtime_error("Unexpected token found" + DEBUG_INFORMATION);
			}
			}

			result.insertNestedInstruction(instruction);
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseNamespace()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Namespace;

		consume(result, TokenType::Namespace, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::Identifier, "Unexpected token found" + DEBUG_INFORMATION);
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"" + DEBUG_INFORMATION);
		result.insertNestedInstruction(parseNamespaceBody());
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
				Instruction instruction;

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
					instruction = parseSymbolBody();
				}
				default:
				{
					insertError("Unexpected token [" + to_string(currentToken().type) + "] found" + DEBUG_INFORMATION);
					throw std::runtime_error("Unexpected token found" + DEBUG_INFORMATION);
				}
				}
				_result.instructions.push_back(instruction);
			}
			catch (std::runtime_error e)
			{
				skipLine();
			}
		}
		return (_result);
	}
}