#pragma once

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "lumina_tokenizer.hpp"
#include "lumina_error.hpp"

namespace Lumina
{
	class Lexer
	{
	public:
		using TokenType = Tokenizer::Token::Type;

		struct Instruction
		{
			std::vector<Tokenizer::Token> tokens;
			std::vector<Instruction> nestedInstructions;

			void insert(const Tokenizer::Token& p_token);
			void insertNestedInstruction(const Instruction& p_instruction);
		};

		struct Result
		{
			std::vector<CompilationError> _errors;
		};

	private:		
		bool _errorActive = true;

		Result _result;
		std::vector<Tokenizer::Token> _tokens;
		size_t _index;

		void setTokenToIndex(size_t p_index);
		bool hasTokenLeft(size_t p_offset = 0) const;
		const Tokenizer::Token& previousToken();
		const Tokenizer::Token& currentToken() const;
		const Tokenizer::Token& nextToken();
		const Tokenizer::Token& tokenAtIndex(size_t index) const;

		void insertError(const std::string& p_error);
		void consume(Instruction& p_instruction);
		void consume(Instruction& p_instruction, TokenType p_expectedTokenType);
		void consume(Instruction& p_instruction, TokenType p_expectedTokenType, const std::string& p_errorMessage);


	public:
		Lexer();
		Result execute(const std::vector<Tokenizer::Token>& p_tokens);

		static Result checkGrammar(const std::vector<Tokenizer::Token>& p_tokens);
	};
}