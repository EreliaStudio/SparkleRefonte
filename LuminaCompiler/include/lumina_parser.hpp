#pragma once

#include "lumina_lexer.hpp"

#include <set>

namespace Lumina
{
	struct Parser
	{
		using InstructionType = Lexer::Instruction::Type;
		struct Result
		{
			std::string vertexShaderCode;
			std::string fragmentShaderCode;
			std::vector<CompilationError> errors;
		};

		std::set<std::string> _alreadyLoadedImport;

		Result _result;
		std::vector<Lexer::Instruction> _instructions;
		size_t _index;

		struct InputBuffer
		{
			std::string type;
			std::string name;
		};

		struct Block
		{
			struct Element
			{
				Element() = default;
				Element(const Lexer::Instruction& p_elementInstruction);

				std::string name;
				std::string type;
			};
			std::string name;
			std::vector<Element> elements;

			Block() = default;
			Block(const std::string& p_blockName, const Lexer::Instruction& p_blockBodyInstruction);
		};

		using Structure = Block;
		using AttributeBlock = Block;
		using ConstantBlock = Block;

		std::vector<InputBuffer> _vertexInputs;
		std::vector<InputBuffer> _fragmentInputs;

		std::vector<Structure> _structures;
		std::vector<AttributeBlock> _attributes;
		std::vector<ConstantBlock> _constants;

		void insertError(const std::string& p_error, const Tokenizer::Token& p_incriminedToken);

		bool hasInstructionLeft();
		const Lexer::Instruction& currentInstruction() const;
		void advance();

		void parseImport();
		void parsePipelineFlow();
		void parseStructure();
		void parseAttribute();
		void parseConstant();
		void parseTexture();
		void parseSymbol();

		Result execute(const std::vector<Lexer::Instruction>& p_instructions);

		static Result validate(const std::vector<Lexer::Instruction>& p_instructions);
	};
}