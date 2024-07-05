#pragma once

#include <iostream>
#include <unordered_map>

#include "lumina_tokenizer.hpp"
#include "lumina_error.hpp"

namespace Lumina
{
	class Lexer
	{
	public:
		struct Instruction
		{
			enum class Type
			{
				Include,
				PipelineFlow,
				Structure,
				Function,
				AttributeBlock,
				ConstantBlock,
				Texture,
				Namespace,
				Comment,
				PipelinePass,
				Expression,
				Variable
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);

			Type type;
			std::vector<Tokenizer::Token> tokens;
			std::vector<Instruction> nestedInstruction;

			friend std::ostream& operator << (std::ostream& p_os, const Instruction& p_instruction);
		};

		struct Result
		{
			std::vector<Instruction> instructions;
			std::vector<CompilationError> errors;
		};
	private:

	public:
		static Result checkGrammar(const std::vector<Tokenizer::Token>& tokens);
	};
}

std::string to_string(Lumina::Lexer::Instruction::Type p_type);