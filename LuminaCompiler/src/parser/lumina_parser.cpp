#include "lumina_parser.hpp"

namespace Lumina
{
	void Parser::parseImport()
	{

	}

	void Parser::parsePipelineFlow()
	{

	}

	void Parser::parseStructure()
	{

	}

	void Parser::parseAttribute()
	{

	}

	void Parser::parseConstant()
	{

	}

	void Parser::parseTexture()
	{

	}

	void Parser::parseSymbol()
	{

	}

	Parser::Result Parser::execute(const std::vector<Lexer::Instruction>& p_instructions)
	{
		_result = Result();
		_instructions = p_instructions;
		_index = 0;

		while (hasInstructionLeft() == true)
		{
			switch (currentInstruction().type)
			{
			case InstructionType::Import:
			{
				parseImport();
				break;
			}
			/*case InstructionType::PipelineFlow:
			{
				parsePipelineFlow();
				break;
			}
			case InstructionType::Structure:
			{
				parseStructure();
				break;
			}
			case InstructionType::Attribute:
			{
				parseAttribute();
				break;
			}
			case InstructionType::Constant:
			{
				parseConstant();
				break;
			}
			case InstructionType::Texture:
			{
				parseTexture();
				break;
			}
			case InstructionType::Symbol:
			{
				parseSymbol();
				break;
			}*/
			default:
			{
				if (currentInstruction().tokens[0].type == Tokenizer::Token::Type::MetaToken)
				{
					insertError("Unexpected instruction detected", currentInstruction().nestedInstructions[currentInstruction().tokens[0].line].tokens[0]);
				}
				else
				{
					insertError("Unexpected instruction detected", currentInstruction().tokens[0]);
				}
				break;
			}
			}
			advance();
		}

		return (_result);
	}
}