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

		result.type = Instruction::Type::PipelineFlow;

		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "Input" }, "Expected a valid pipeline flow");
		consume(result, TokenType::PipelineSeparator, "Expected a token \"->\"");
		consumeMultiple(result, TokenType::PipelineFlow, { "VertexPass", "FragmentPass" }, "Expected a valid pipeline flow");
		consume(result, TokenType::Separator, "Expected a token \":\"");
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parsePipelineDefinition()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::PipelineDefinition;

		consumeMultiple(result, TokenType::PipelineFlow, {"VertexPass", "FragmentPass"}, "Expected a valid pipeline pass definition");
		consume(result, TokenType::OpenParenthesis, "Expected a token \"(\"");
		consume(result, TokenType::ClosedParenthesis, "Expected a token \")\"");
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
		result.insertNestedInstruction(parseSymbolBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseImport()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Import;

		consume(result, TokenType::Import, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::StringLitterals, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));

		return (result);
	}

	Lexer::Instruction Lexer::parseCallParameters()
	{
		Instruction result;

		result.type = Instruction::Type::CallParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			result.insertNestedInstruction(parseExpression());
			if (currentToken().type != TokenType::ClosedParenthesis)
				consume(result, TokenType::Comma, "Expected parameter separator \",\"");
		}

		return (result);
	}

	void Lexer::expendExpression(Lexer::Instruction& p_instruction)
	{
		switch (currentToken().type)
		{
		case TokenType::Number:
		{
			consume(p_instruction, TokenType::Number, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			break;
		}
		case TokenType::Identifier:
		{
			consume(p_instruction, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			if (currentToken().type == TokenType::OpenParenthesis)
			{
				consume(p_instruction, TokenType::OpenParenthesis, "Expected body opener token \"(\"");
				p_instruction.insertNestedInstruction(parseCallParameters());
				consume(p_instruction, TokenType::ClosedParenthesis, "Expected body opener token \")\"");
			}
			break;
		}
		}
	}

	Lexer::Instruction Lexer::parseExpression()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Expression;

		expendExpression(result);
		while (currentToken().type == TokenType::Operator)
		{
			consume(result, TokenType::Operator, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			expendExpression(result);
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseBlockBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

		while (currentToken().type != TokenType::ClosedCurlyBracket)
		{
			consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			if (currentToken().type == TokenType::Assignator)
			{
				consume(result, TokenType::Assignator, "Expected an assignator token \"=\"");
				result.insertNestedInstruction(parseExpression());
			}
			consume(result, TokenType::EndOfSentence, "Expected a token \";\"");
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseStructureBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::Structure, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
		result.insertNestedInstruction(parseBlockBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}
	
	Lexer::Instruction Lexer::parseAttributeBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::AttributeBlock, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
		result.insertNestedInstruction(parseBlockBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseConstantBlock()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Structure;

		consume(result, TokenType::ConstantBlock, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
		result.insertNestedInstruction(parseBlockBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseTexture()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Texture;

		consume(result, TokenType::Texture, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::EndOfSentence, "Expected a token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseSymbolParameters()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::SymbolParameters;

		while (currentToken().type != TokenType::ClosedParenthesis)
		{
			consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
			if (currentToken().type != TokenType::ClosedParenthesis)
				consume(result, TokenType::Comma, "Expected a token \",\"");
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseIfStatement()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::IfStatement;



		return (result);
	}
	
	Lexer::Instruction Lexer::parseWhileStatement()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::WhileStatement;

		return (result);
	}
	
	Lexer::Instruction Lexer::parseForStatement()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::ForStatement;

		return (result);
	}

	Lexer::Instruction Lexer::parseSymbolBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

		int nbCurly = 1;
		do
		{
			if (currentToken().type == TokenType::OpenCurlyBracket)
			{
				nbCurly++;
			}
			
			switch (currentToken().type)
			{
			/*case TokenType::IfStatement:
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
			case TokenType::OpenCurlyBracket:
			{
				consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
				result.insertNestedInstruction(parseSymbolBody());
				consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");
				break;
			}*/
			default:
			{
				skipToken();
				break;
			}
			}
			
			if (currentToken().type == TokenType::ClosedCurlyBracket)
			{
				nbCurly--;
			}

		}
		while (currentToken().type != TokenType::ClosedCurlyBracket || nbCurly != 0);

		return (result);
	}

	Lexer::Instruction Lexer::parseSymbol()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Symbol;

		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::OpenParenthesis, "Expected body opener token \"(\"");
		result.insertNestedInstruction(parseSymbolParameters());
		consume(result, TokenType::ClosedParenthesis, "Expected body opener token \")\"");
		if (currentToken().type == TokenType::EndOfSentence)
		{
			consume(result, TokenType::EndOfSentence, "Expected a token \";\"");
		}
		else
		{
			consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
			result.insertNestedInstruction(parseSymbolBody());
			consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");
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
				insertError("Unexpected token [" + to_string(currentToken().type) + "] found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
				throw std::runtime_error("Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
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

		consume(result, TokenType::Namespace, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::Identifier, "Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
		consume(result, TokenType::OpenCurlyBracket, "Expected body opener token \"{\"");
		result.insertNestedInstruction(parseNamespaceBody());
		consume(result, TokenType::ClosedCurlyBracket, "Expected body closer token \"}\"");

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
				default:
				{
					insertError("Unexpected token [" + to_string(currentToken().type) + "] found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
					throw std::runtime_error("Unexpected token found " + std::string(__FUNCTION__) + "::" + std::to_string(__LINE__));
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