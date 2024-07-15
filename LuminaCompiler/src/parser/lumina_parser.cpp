#include "lumina_parser.hpp"

#include "lumina_utils.hpp"

namespace Lumina
{
	void Parser::parseImport()
	{
		std::string importName = currentInstruction().tokens[1].content;

		std::string importFilePath = composeFilePath(importName.substr(1, importName.size() - 2)).string();

		if (importFilePath.size() == 0)
		{
			insertError("Import file [" + currentInstruction().tokens[1].content + "] not found", currentInstruction().tokens[1]);
			throw std::runtime_error("Invalid header file");
		}

		if (_alreadyLoadedImport.contains(importFilePath) == false)
		{
			std::vector<Tokenizer::Token> tokens = Tokenizer::tokenize(readFileAsString(importFilePath));

			Lexer::Result lexerResult = Lexer::checkGrammar(tokens);

			if (lexerResult.errors.size() != 0)
			{
				for (auto& error : lexerResult.errors)
				{
					_result.errors.push_back(CompilationError("In file [" + importName.substr(1, importName.size() - 2) + "] - " + error.message(), error.lineIndex(), error.line(), error.column(), error.tokenSize()));
				}

				throw std::runtime_error("Error in header file");

			}

			_instructions.insert(_instructions.begin() + _index, lexerResult.instructions.begin(), lexerResult.instructions.end());

			_alreadyLoadedImport.insert(importFilePath);
		}
	}

	void Parser::parsePipelineFlow()
	{
		if (currentInstruction().tokens[0].content == "Input")
		{
			if (currentInstruction().tokens[2].content != "VertexPass")
			{
				insertError("Invalid pipeline flow ouput : Input can only provide for VertexPass", currentInstruction().tokens[2]);
				throw std::runtime_error("Error in header file");
			}
			_vertexInputs.push_back({ currentInstruction().tokens[4].content, currentInstruction().tokens[5].content });
		}
		else if (currentInstruction().tokens[0].content == "VertexPass")
		{
			if (currentInstruction().tokens[2].content != "FragmentPass")
			{
				insertError("Invalid pipeline flow ouput : VertexPass can only provide for FragmentPass", currentInstruction().tokens[2]);
				throw std::runtime_error("Error in header file");
			}
			_fragmentInputs.push_back({ currentInstruction().tokens[4].content, currentInstruction().tokens[5].content });
		}
	}

	Parser::Block::Element::Element(const Lexer::Instruction& p_elementInstruction)
	{
		type = p_elementInstruction.tokens[0].content;
		name = p_elementInstruction.tokens[1].content;
	}

	Parser::Block::Block(const std::string& p_blockName, const Lexer::Instruction& p_blockBodyInstruction)
	{
		name = p_blockName;
		for (const auto& elementInstruction : p_blockBodyInstruction.nestedInstructions)
		{
			elements.push_back(Block::Element(elementInstruction));
		}
	}

	void Parser::parseStructure()
	{
		for (const auto& structure : _structures)
		{
			if (structure.name == currentInstruction().tokens[1].content)
			{
				insertError("Structure [" + currentInstruction().tokens[1].content + "] already defined", currentInstruction().tokens[1]);
				throw std::runtime_error("Error in header file");
			}
		}

		_structures.push_back(Block(currentInstruction().tokens[1].content, currentInstruction().nestedInstructions[0]));
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
			try
			{
			switch (currentInstruction().type)
			{
			case InstructionType::Import:
			{
				parseImport();
				break;
			}
			case InstructionType::PipelineFlow:
			{
				parsePipelineFlow();
				break;
			}
			case InstructionType::Structure:
			{
				parseStructure();
				break;
			}
			/*case InstructionType::Attribute:
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

			}
			catch (...)
			{
				
			}
			advance();
		}

		return (_result);
	}
}