#pragma once

#include "lumina_lexer.hpp"

namespace Lumina
{
	struct Parser
	{
		using InstructionType = Lexer::Instruction::Type;
		struct Result
		{
			std::vector<CompilationError> errors;
		};

		Result _result;
		std::vector<Lexer::Instruction> _instructions;
		size_t _index;

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