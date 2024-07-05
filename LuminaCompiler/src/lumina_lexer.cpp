#include "lumina_lexer.hpp"

#include <iomanip>

#define DEBUG_LINE() std::cout << __FUNCTION__ << "::" << __LINE__ << std::endl

namespace Lumina
{
	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction::Type& p_type)
	{
		switch (p_type)
		{
		case Lexer::Instruction::Type::Include:
			p_os << "Include";
			break;
		case Lexer::Instruction::Type::PipelineFlow:
			p_os << "PipelineFlow";
			break;
		case Lexer::Instruction::Type::Structure:
			p_os << "Structure";
			break;
		case Lexer::Instruction::Type::Function:
			p_os << "Function";
			break;
		case Lexer::Instruction::Type::AttributeBlock:
			p_os << "AttributeBlock";
			break;
		case Lexer::Instruction::Type::ConstantBlock:
			p_os << "ConstantBlock";
			break;
		case Lexer::Instruction::Type::Namespace:
			p_os << "Namespace";
			break;
		case Lexer::Instruction::Type::Comment:
			p_os << "Comment";
			break;
		case Lexer::Instruction::Type::PipelinePass:
			p_os << "PipelinePass";
			break;
		case Lexer::Instruction::Type::Expression:
			p_os << "Expression";
			break;
		case Lexer::Instruction::Type::Variable:
			p_os << "Variable";
			break;
		default:
			p_os << "Unknown";
			break;
		}
		return p_os;
	}

	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction& p_instruction)
	{
		p_os << "Type [" << std::setw(15) << p_instruction.type << "] - ";
		if (p_instruction.tokens.size() == 0)
		{
			p_os << "No token";
			return (p_os);
		}

		size_t currentLine = p_instruction.tokens[0].line;
		for (size_t i = 0; i < p_instruction.tokens.size(); i++)
		{
			if (p_instruction.tokens[i].line != currentLine)
			{
				p_os << "\n";
				currentLine = p_instruction.tokens[i].line;
			}
			if (i != 0)
				p_os << " ";
			p_os << p_instruction.tokens[i].content;
		}

		return (p_os);
	}

	void skipWord(const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		index++;
	}

	void skipLine(const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		size_t currentLine = tokens[index].line;

		while (index < tokens.size() && tokens[index].line == currentLine)
		{
			index++;
		}
	}

	void skipComment(const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		if (tokens[index].type != Tokenizer::Token::Type::MultilineComment && tokens[index].type != Tokenizer::Token::Type::SingleLineComment)
			return;
		if (tokens[index].type == Tokenizer::Token::Type::SingleLineComment)
			skipLine(tokens, index);
		else
		{
			while (index < tokens.size() && tokens[index].type != Tokenizer::Token::Type::EndOfMultilineComment)
			{
				index++;
			}
			index++;
		}
	}

	void insertError(Lexer::Result& result, const std::string& errorMessage, const Tokenizer::Token& token)
	{
		result.errors.push_back(CompilationError(errorMessage, token.line, token.fullLine, token.column, token.content.size()));
	}

	Tokenizer::Token createMetaToken(size_t metaTokenID)
	{
		Tokenizer::Token result;

		result.type = Tokenizer::Token::Type::MetaToken;
		result.line = metaTokenID;

		return (result);
	}

	void consume(Lexer::Result& result, Lexer::Instruction& newInstruction, const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Token::Type type, const std::string& errorMessage)
	{
		if (tokens[index].type != type)
		{
			insertError(result, errorMessage, tokens[index]);
			skipLine(tokens, index);
			throw std::runtime_error("Invalid grammar detected");
		}
		newInstruction.tokens.push_back(tokens[index]);
		index++;
	}

	void consume(Lexer::Result& result, Lexer::Instruction& newInstruction, const std::vector<Tokenizer::Token>& tokens, size_t& index, Tokenizer::Token::Type type, std::vector<std::string> possibleValues, const std::string& errorMessage)
	{
		if (tokens[index].type != type)
		{
			insertError(result, errorMessage, tokens[index]);
			skipLine(tokens, index);
			throw std::runtime_error("Invalid grammar detected");
		}
		bool found = false;
		for (const auto& tmp : possibleValues)
		{
			if (tmp == tokens[index].content)
				found = true;
		}
		if (found == false)
		{
			std::string invalidValueMessage = "Unexpected value. Accepted values : ";
			for (const auto& tmp : possibleValues)
			{
				invalidValueMessage += "[" + tmp + "]";
			}
			insertError(result, invalidValueMessage, tokens[index]);
			skipLine(tokens, index);
			throw std::runtime_error("Invalid grammar detected");
		}
		newInstruction.tokens.push_back(tokens[index]);
		index++;
	}

	void consume(Lexer::Result& result, Lexer::Instruction& newInstruction, const std::vector<Tokenizer::Token>& tokens, size_t& index, std::vector<Tokenizer::Token::Type> types, const std::string& errorMessage)
	{
		bool found = false;
		for (const auto& type : types)
		{
			if (type == tokens[index].type)
				found = true;
		}
		if (found == false)
		{
			insertError(result, errorMessage, tokens[index]);
			skipLine(tokens, index);
			throw std::runtime_error("Invalid grammar detected");
		}
		newInstruction.tokens.push_back(tokens[index]);
		index++;
	}

	void createNestedInstruction(Lexer::Instruction& instruction, const Lexer::Instruction& nestedInstruction)
	{
		instruction.tokens.push_back(createMetaToken(instruction.nestedInstruction.size()));
		instruction.nestedInstruction.push_back(nestedInstruction);
	}

	void parseInclude(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::Include;
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Include, "Expected an include keyword");

		if (tokens[index].type == Tokenizer::Token::Type::StringLiteral)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::StringLiteral, "Expected a '<' or a \"");
		}
		else if (tokens[index].type == Tokenizer::Token::Type::Operator &&
				tokens[index + 1].type == Tokenizer::Token::Type::Identifier &&
				tokens[index + 2].type == Tokenizer::Token::Type::Operator)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Operator, "Expected a '<' or a \"");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a include path");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Operator, "Expected a '>' or a \"");
		}
		else
		{
			insertError(result, "Unexpected include token", tokens[index]);
			skipLine(tokens, index);
			return;
		}
	}

	Lexer::Instruction parseVariableDeclaration(Lexer::Result& result, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		Lexer::Instruction instruction;
		instruction.type = Lexer::Instruction::Type::Variable;

		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected value type");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected value name");

		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
		skipComment(tokens, index);

		return (instruction);
	}

	void parseStructure(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::Structure;

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Keyword, {"struct"}, "Expected a \"struct\" Keyword ");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected name");
		skipComment(tokens, index);

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyOpener, "Expected a '{'");

		skipComment(tokens, index);
		if (tokens[index].type != Tokenizer::Token::Type::BodyCloser)
		{
			skipComment(tokens, index);
			while (tokens[index].type == Tokenizer::Token::Type::Identifier)
			{
				createNestedInstruction(instruction, parseVariableDeclaration(result, tokens, index));
			}
		}

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyCloser, "Expected a '}'");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
	}

	Lexer::Instruction parseExpression(Lexer::Result& result, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		Lexer::Instruction instruction;
		instruction.type = Lexer::Instruction::Type::Expression;

		if (tokens[index].type == Tokenizer::Token::Type::Number)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Number, "Unexpected value");
		}
		else if (tokens[index].type == Tokenizer::Token::Type::Identifier)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected type name");
			if (tokens[index].type == Tokenizer::Token::Type::ParenthesisOpener)
			{
				consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisOpener, "Expected a '('");
				do
				{
					createNestedInstruction(instruction, parseExpression(result, tokens, index));

					if (tokens[index].type != Tokenizer::Token::Type::ParenthesisCloser)
						consume(result, instruction, tokens, index, Tokenizer::Token::Type::Comma, "Expected a ',' before the next parameter");

				} while (tokens[index].type != Tokenizer::Token::Type::ParenthesisCloser);
				consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisCloser, "Expected a ')'");
			}
		}

		return (instruction);
	}

	Lexer::Instruction parseVariableDeclarationInitialisation(Lexer::Result& result, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		Lexer::Instruction instruction;

		instruction.type = Lexer::Instruction::Type::Variable;

		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected value type");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected value name");

		skipComment(tokens, index);
		
		if (tokens[index].type == Tokenizer::Token::Type::Assignator)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Assignator, "Expected a '='");

			createNestedInstruction(instruction, parseExpression(result, tokens, index));
		}

		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
		skipComment(tokens, index);

		return (instruction);
	}
	
	void parseBlock(Lexer::Result& result, Lexer::Instruction& instruction, Lexer::Instruction::Type type, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = type;

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Keyword, { "AttributeBlock", "ConstantBlock"}, "Expected a Keyword \"AttributeBlock\" or \"ConstantBlock\"");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected block name");
		skipComment(tokens, index);

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyOpener, "Expected a '{'");

		skipComment(tokens, index);
		if (tokens[index].type != Tokenizer::Token::Type::BodyCloser)
		{
			skipComment(tokens, index);
			while (tokens[index].type == Tokenizer::Token::Type::Identifier)
			{
				Lexer::Instruction variableInstruction;

				createNestedInstruction(instruction, parseVariableDeclarationInitialisation(result, tokens, index));

				instruction.nestedInstruction.push_back(variableInstruction);

			}
		}

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyCloser, "Expected a '}'");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
	}

	void parseTexture(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::Texture;
		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Keyword, { "Texture" }, "Expected \"Texture\" keyword");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a texture name");

		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
		skipComment(tokens, index);
	}

	void parseParameters(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		if (tokens[index].type == Tokenizer::Token::Type::ParenthesisCloser)
			return;

		do
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a parameter type");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a parameter name");

			if (tokens[index].type != Tokenizer::Token::Type::ParenthesisCloser)
				consume(result, instruction, tokens, index, Tokenizer::Token::Type::Comma, "Expected a ',' before the next parameter");

		} while (tokens[index].type != Tokenizer::Token::Type::ParenthesisCloser);
	}

	void parseFunctionBody(Lexer::Result& result, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		int nbParenthesis = 1;
		do
		{
			skipWord(tokens, index);

			if (tokens[index].type == Tokenizer::Token::Type::BodyOpener)
				nbParenthesis++;
			if (tokens[index].type == Tokenizer::Token::Type::BodyCloser)
				nbParenthesis--;

		} while (tokens[index].type != Tokenizer::Token::Type::BodyCloser || nbParenthesis != 0);
	}

	void parseFunction(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::Function;

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a return type");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected a function name");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisOpener, "Expected '('");
		parseParameters(result, instruction, tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisCloser, "Expected ')'");
		skipComment(tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyOpener, "Expected '{'");
		parseFunctionBody(result, tokens, index);
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyCloser, "Expected '}'");
	}

	void parseNamespace(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::Namespace;

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Keyword, { "namespace" }, "Expected a Keyword \"namespace\"");
		consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Expected namespace name");
		skipComment(tokens, index);

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyOpener, "Expected a '{'");

		while (tokens[index].type != Tokenizer::Token::Type::BodyCloser)
		{
			skipComment(tokens, index);
			Lexer::Instruction newInstruction;

			if (tokens[index].type == Tokenizer::Token::Type::Keyword)
			{

				if (tokens[index].content == "struct")
				{
					parseStructure(result, newInstruction, tokens, index);
				}
				else if (tokens[index].content == "AttributeBlock")
				{
					parseBlock(result, newInstruction, Lexer::Instruction::Type::AttributeBlock, tokens, index);
				}
				else if (tokens[index].content == "ConstantBlock")
				{
					parseBlock(result, newInstruction, Lexer::Instruction::Type::ConstantBlock, tokens, index);
				}
				else if (tokens[index].content == "Texture")
				{
					parseTexture(result, newInstruction, tokens, index);
				}
				else
				{
					insertError(result, "Unexpected token", tokens[index]);
					skipLine(tokens, index);
					throw std::runtime_error("Invalid grammar detected");
				}
			}
			else if (tokens[index].type == Tokenizer::Token::Type::Identifier)
			{
				parseFunction(result, newInstruction, tokens, index);
			}
			else
			{
				insertError(result, "Token [" + tokens[index].content + "](" + to_string(tokens[index].type) + ") not recognized", tokens[index]);
				skipLine(tokens, index);
				throw std::runtime_error("Invalid grammar detected");
			}

			createNestedInstruction(instruction, newInstruction);
		}

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyCloser, "Expected a '}'");
	}



	void parsePipelineFlow(Lexer::Result& result, Lexer::Instruction& instruction, const std::vector<Tokenizer::Token>& tokens, size_t& index)
	{
		instruction.type = Lexer::Instruction::Type::PipelineFlow;

		consume(result, instruction, tokens, index, Tokenizer::Token::Type::PipelineFlow, "Expected a PipelineFlow");

		if (tokens[index].type == Tokenizer::Token::Type::PipelineFlowSeparator)
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::PipelineFlowSeparator, "Expected a pipeline separator");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::PipelineFlow, "Expected a PipelineFlow");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Separator, "Expected a ':'");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected type");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::Identifier, "Unexpected name");

			skipComment(tokens, index);

			consume(result, instruction, tokens, index, Tokenizer::Token::Type::EndOfSentence, "Expected a ';'");
		}
		else
		{
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisOpener, "Expected '('");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::ParenthesisCloser, "Expected ')'");
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyOpener, "Expected '{'");
			parseFunctionBody(result, tokens, index);
			consume(result, instruction, tokens, index, Tokenizer::Token::Type::BodyCloser, "Expected '}'");
		}
		

	}

	Lexer::Result Lexer::checkGrammar(const std::vector<Tokenizer::Token>& tokens)
	{
		Lexer::Result result;

		size_t index = 0;
		while (index < tokens.size())
		{
			try
			{
				bool parsed = true;
				Lexer::Instruction newInstruction;

				switch (tokens[index].type)
				{
				case Tokenizer::Token::Type::Include:
				{
					parseInclude(result, newInstruction, tokens, index);
					break;
				}
				case Tokenizer::Token::Type::SingleLineComment:
				{
					skipLine(tokens, index);
					parsed = false;
					break;
				}
				case Tokenizer::Token::Type::MultilineComment:
				{
					skipComment(tokens, index);
					parsed = false;
					break;
				}
				case Tokenizer::Token::Type::PipelineFlow:
				{
					parsePipelineFlow(result, newInstruction, tokens, index);
					break;
				}
				case Tokenizer::Token::Type::Keyword:
				{
					if (tokens[index].content == "struct")
					{
						parseStructure(result, newInstruction, tokens, index);
					}
					else if (tokens[index].content == "namespace")
					{
						parseNamespace(result, newInstruction, tokens, index);
					}
					else if (tokens[index].content == "AttributeBlock")
					{
						parseBlock(result, newInstruction, Lexer::Instruction::Type::AttributeBlock, tokens, index);
					}
					else if (tokens[index].content == "ConstantBlock")
					{
						parseBlock(result, newInstruction, Lexer::Instruction::Type::ConstantBlock, tokens, index);
					}
					else if (tokens[index].content == "Texture")
					{
						parseTexture(result, newInstruction, tokens, index);
					}
					else
					{
						insertError(result, "Unexpected keyword", tokens[index]);
						skipLine(tokens, index);
						parsed = false;
					}
					break;
				}
				default:
				{
					insertError(result, "Token [" + tokens[index].content + "](" + to_string(tokens[index].type) + ") not recognized", tokens[index]);
					skipLine(tokens, index);
					parsed = false;
					break;
				}
				}
				if (parsed == true)
					result.instructions.push_back(newInstruction);
			}
			catch (...)
			{

			}
			
		}

		return (result);
	}

}

std::string to_string(Lumina::Lexer::Instruction::Type p_type)
{
	switch (p_type)
	{
	case Lumina::Lexer::Instruction::Type::Include:
		return "Include";
	case Lumina::Lexer::Instruction::Type::PipelineFlow:
		return "PipelineFlow";
	case Lumina::Lexer::Instruction::Type::Structure:
		return "Structure";
	case Lumina::Lexer::Instruction::Type::Function:
		return "Function";
	case Lumina::Lexer::Instruction::Type::AttributeBlock:
		return "AttributeBlock";
	case Lumina::Lexer::Instruction::Type::ConstantBlock:
		return "ConstantBlock";
	case Lumina::Lexer::Instruction::Type::Texture:
		return "Texture";
	case Lumina::Lexer::Instruction::Type::Namespace:
		return "Namespace";
	case Lumina::Lexer::Instruction::Type::Comment:
		return "Comment";
	case Lumina::Lexer::Instruction::Type::PipelinePass:
		return "PipelinePass";
	case Lumina::Lexer::Instruction::Type::Expression:
		return "Expression";
	case Lumina::Lexer::Instruction::Type::Variable:
		return "Variable";
	default:
		return "Unknown";
	}
}