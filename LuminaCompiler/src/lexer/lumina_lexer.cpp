#include "lumina_lexer.hpp"

namespace Lumina
{

	Lexer::Instruction Lexer::parseInclude()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Include;

		consumeAndCheckValue(result, TokenType::Include, "#include", "Expected a #include token");
		switch (currentToken().type)
		{
		case TokenType::StringLiteral:
		{
			consumeOnSameLine(result, TokenType::StringLiteral, "Expected a valide include path");
			break;
		}
		case TokenType::Operator:
		{
			consumeOnSameLineAndCheckValue(result, TokenType::Operator, "<", "Expected a \"<\"");

			while (currentToken().type != TokenType::Operator || currentToken().content != ">")
			{
				consumeOnSameLine(result);
			}

			consumeOnSameLineAndCheckValue(result, TokenType::Operator, ">", "Expected a \">\"");
			break;
		}
		default:
		{
			insertError("Unexpected token found");
			throw std::runtime_error("Unexpected error");
		}
		}

		return (result);
	}

	Lexer::Instruction Lexer::parsePipelineFlow()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::PipelineFlow;

		consumeAndCheckValues(result, TokenType::PipelineFlow, {"Input", "VertexPass"}, "Expected a valid pipeline name");
		consumeOnSameLine(result, TokenType::PipelineFlowSeparator, "Expected separator \"->\" not found");
		consumeOnSameLineAndCheckValues(result, TokenType::PipelineFlow, { "FragmentPass", "VertexPass" }, "Expected a valid pipeline name");
		consumeOnSameLine(result, TokenType::Separator, "Expected pipeline flow separator \":\" not found");
		consumeOnSameLine(result, TokenType::Identifier, "Expected typename not found");
		consumeOnSameLine(result, TokenType::Identifier, "Expected variable name not found");
		consumeOnSameLine(result, TokenType::EndOfSentence, "Missing token \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseExpression()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Expression;

		if (currentToken().type == TokenType::Number)
		{
			consume(result, TokenType::Number, "Expected a valid RValue expression");
		}
		else
		{
			consume(result, TokenType::Identifier, "Expected a variable name");
			switch (currentToken().type)
			{
				case TokenType::Accessor:
				{
					while (currentToken().type == TokenType::Accessor)
					{
						consume(result, TokenType::Accessor, "Expected an accessor operator \".\"");
						consume(result, TokenType::Identifier, "Expected a member name");
					}
					break;
				}
				case TokenType::ParenthesisOpener:
				{
					consume(result, TokenType::ParenthesisOpener, "Expected a \"(\" operator");

					while (currentToken().type != TokenType::ParenthesisCloser)
					{
						result.addNestedInstruction(parseExpression());
						if (currentToken().type != TokenType::ParenthesisCloser)
							consume(result, TokenType::Comma, "Expected an comma separator \",\"");
					}

					consume(result, TokenType::ParenthesisCloser, "Expected a \")\" operator");
					break;
				}
			}
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseRValue()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::RValue;

		while (currentToken().type != TokenType::EndOfSentence)
		{
			result.addNestedInstruction(parseExpression());

			if (currentToken().type != TokenType::EndOfSentence)
			{
				consumeAndCheckValues(result, TokenType::Operator, {"+", "-", "*", "/", "%"}, "Expected a valid RValue operator");
			}
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseMember()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Member;

		consume(result, TokenType::Identifier, "Expected a variable type");
		consume(result, TokenType::Identifier, "Expected a variable name");
		if (currentToken().type != TokenType::EndOfSentence)
		{
			consume(result, TokenType::Assignator, "Expected an assignement operator \"=\" after a variable declaration");
			result.addNestedInstruction(parseRValue());
		}
		consume(result, TokenType::EndOfSentence, "Expected a \";\"");
		
		return (result);
	}

	void Lexer::parseBlockBody(Instruction& p_instruction, bool p_canAssign)
	{
		Lexer::Instruction bodyInstruction;

		bodyInstruction.type = Lexer::Instruction::Type::Body;

		consume(bodyInstruction, TokenType::BodyOpener, "Expected \"{\"");
		while (currentToken().type != TokenType::BodyCloser)
		{
			bodyInstruction.addNestedInstruction(parseMember());

			
		}
		consume(bodyInstruction, TokenType::BodyCloser, "Expected \"}\"");
		consume(bodyInstruction, TokenType::EndOfSentence, "Expected \";\"");

		p_instruction.addNestedInstruction(bodyInstruction);
	}

	Lexer::Instruction Lexer::parseStructureBlock()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Structure;

		consumeAndCheckValue(result, TokenType::Keyword, "struct", "Expected keyword \"struct\"");
		consume(result, TokenType::Identifier, "Expected structure name");
		
		parseBlockBody(result, false);

		return (result);
	}

	Lexer::Instruction Lexer::parseAttributeBlock()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::AttributeBlock;

		consumeAndCheckValue(result, TokenType::Keyword, "AttributeBlock", "Expected keyword \"AttributeBlock\"");
		consume(result, TokenType::Identifier, "Expected an attribute block name");

		parseBlockBody(result, true);

		return (result);
	}

	Lexer::Instruction Lexer::parseConstantBlock()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::ConstantBlock;

		consumeAndCheckValue(result, TokenType::Keyword, "ConstantBlock", "Expected keyword \"ConstantBlock\"");
		consume(result, TokenType::Identifier, "Expected an constant block name");

		parseBlockBody(result, true);

		return (result);
	}

	Lexer::Instruction Lexer::parseParameter()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Parameter;

		consume(result, TokenType::Identifier, "Expected a parameter type");
		consume(result, TokenType::Identifier, "Expected a parameter name");

		return (result);
	}
	
	Lexer::Instruction Lexer::parseSymbol()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Symbol;

		consume(result, TokenType::Identifier, "Expected a return type");
		consume(result, TokenType::Identifier, "Expected a function name");
		consume(result, TokenType::ParenthesisOpener, "Expected \"(\"");
		while (currentToken().type != TokenType::ParenthesisCloser)
		{
			result.addNestedInstruction(parseParameter());
			if (currentToken().type != TokenType::ParenthesisCloser)
				consume(result, TokenType::Comma, "Expected comma separator \",\"");
		}
		consume(result, TokenType::ParenthesisCloser, "Expected \")\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseFunctionBody()
	{
		Lexer::Instruction result;

		result.type = Instruction::Type::Body;

		while (currentToken().type != TokenType::BodyCloser)
		{
			skipWord();
		}

		return (result);
	}

	Lexer::Instruction Lexer::parseFunction()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Function;

		result.addNestedInstruction(parseSymbol());

		if (currentToken().type == TokenType::EndOfSentence)
		{
			consume(result, TokenType::EndOfSentence, "Expected \";\"");
		}
		else
		{
			consume(result, TokenType::BodyOpener, "Expected \"{\"");
			result.addNestedInstruction(parseFunctionBody());
			consume(result, TokenType::BodyCloser, "Expected \"}\"");
		}

		//insertError(__FUNCTION__ + std::string(" not implemented - Token type [" + to_string(currentToken().type) + "]"));
		//throw std::runtime_error(__FUNCTION__ + std::string(" not completed"));

		return (result);
	}

	Lexer::Instruction Lexer::parseTexture()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Texture;

		consumeAndCheckValue(result, TokenType::Keyword, "Texture", "Expected keyword \"Texture\"");
		consume(result, TokenType::Identifier, "Expected an variable name");
		consume(result, TokenType::EndOfSentence, "Expected \";\"");

		return (result);
	}

	Lexer::Instruction Lexer::parseNamespace()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::Namespace;

		consumeAndCheckValue(result, TokenType::Keyword, "namespace", "Expected keyword \"namespace\"");
		consume(result, TokenType::Identifier, "Expected an namespace name");

		consume(result, TokenType::BodyOpener, "Expected \"{\"");

		while (currentToken().type != TokenType::BodyCloser)
		{
			switch (currentToken().type)
			{
			case TokenType::Keyword:
			{
				if (currentToken().content == "struct")
					result.addNestedInstruction(parseStructureBlock());
				else if (currentToken().content == "AttributeBlock")
					result.addNestedInstruction(parseAttributeBlock());
				else if (currentToken().content == "ConstantBlock")
					result.addNestedInstruction(parseConstantBlock());
				else if (currentToken().content == "Namespace")
					result.addNestedInstruction(parseConstantBlock());
				else if (currentToken().content == "Texture")
					result.addNestedInstruction(parseTexture());
				else
				{
					insertError("Unexpected keyword [" + currentToken().content + "] found in namespace body");
					throw std::runtime_error("Unexpected error");
				}
				break;
			}
			case TokenType::Identifier:
			{
				result.addNestedInstruction(parseFunction());
				break;
			}
			default:
			{
				insertError(__FUNCTION__ + std::string(" not implemented - Token type [" + to_string(currentToken().type) + "]"));
				throw std::runtime_error(__FUNCTION__ + std::string(" not completed"));
			}
			}
		}

		consume(result, TokenType::BodyCloser, "Expected \"}\"");


		return (result);
	}

	Lexer::Instruction Lexer::parsePipelineDefinition()
	{
		Lexer::Instruction result;

		result.type = Lexer::Instruction::Type::PipelinePass;

		insertError(__FUNCTION__ + std::string(" not implemented"));
		throw std::runtime_error(__FUNCTION__ + std::string(" not completed"));

		return (result);
	}

	void Lexer::execute(const std::vector<Tokenizer::Token>& p_tokens)
	{
		_index = 0;
		_tokens = p_tokens;
		_result = Result();

		while (hasTokenLeft() == true)
		{
			Lexer::Instruction instruction;

			try
			{
				switch (currentToken().type)
				{
				case TokenType::SingleLineComment:
				case TokenType::MultilineComment:
				{
					skipComment();
					break;
				}
				case TokenType::Include:
				{
					appendInstruction(parseInclude());
					break;
				}
				case TokenType::PipelineFlow:
				{
					if (nextToken().type == TokenType::PipelineFlowSeparator)
						appendInstruction(parsePipelineFlow());
					else
						appendInstruction(parsePipelineDefinition());
					break;
				}
				case TokenType::Keyword:
				{
					if (currentToken().content == "struct")
						appendInstruction(parseStructureBlock());
					else if (currentToken().content == "AttributeBlock")
						appendInstruction(parseAttributeBlock());
					else if (currentToken().content == "ConstantBlock")
						appendInstruction(parseConstantBlock());
					else if (currentToken().content == "namespace")
						appendInstruction(parseNamespace());
					else if (currentToken().content == "Texture")
						appendInstruction(parseTexture());
					else
					{
						insertError("Unexpected keyword [" + currentToken().content + "] found in pipeline code");
						throw std::runtime_error("Unexpected error");
					}
					break;
				}
				case TokenType::Identifier:
				{
					appendInstruction(parseFunction());
				}
				default:
				{
					insertError(__FUNCTION__ + std::string(" not implemented"));
					throw std::runtime_error(__FUNCTION__ + std::string(" not completed"));
				}
				}
			}
			catch (...)
			{
				skipLine();
				continue;
			}
		}
	}

	const Lexer::Result& Lexer::result() const
	{
		return (_result);
	}

	Lexer::Result Lexer::checkGrammar(const std::vector<Tokenizer::Token>& p_tokens)
	{
		Lexer lexer;

		lexer.execute(p_tokens);

		return (lexer.result());
	}
}