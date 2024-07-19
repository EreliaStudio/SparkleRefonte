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

		/*
			MyNamespace::MyType myVariable = symbolCall(parameterA, 42, symbalCall2()) + 45;
			if (myVariable.x == expectedValue(15))
			{
				myVariable.w = 42;
			}

			First instruction :
			MyNamespace::MyType myVariable
			|----------------------------|
				 Variable Declaration
			|-----------------| |--------|
					Type           name

			Second instruction :
								myVariable = symbolCall(parameterA, 42, symbalCall2()) +  45;
								|----------------------------------------------------------|
												Variable assignation
								|--------|   |---------------------------------------------|
								   name 		              RValue
											 |---------------------------------------| || ||
								                            symbolCall                 op Num
											 |--------| |--------|  ||  |------------|
												name       value    Num   symbolCall

			
			*/

		struct Result
		{
			std::vector<CompilationError> errors;
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
		void skipToken();
		void skipLine();
		void skipComment();


	public:
		Lexer();
		Result execute(const std::vector<Tokenizer::Token>& p_tokens);

		static Result checkGrammar(const std::vector<Tokenizer::Token>& p_tokens);
	};
}