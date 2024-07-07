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
		using TokenType = Tokenizer::Token::Type;

		struct Instruction
		{
			enum class Type
			{
				Include, //#include 
				PipelineFlow, //Input -> VertexPass etc
				Structure, //struct
				Function, // function name and prototype
				AttributeBlock, // AttributeBlock
				ConstantBlock, //ConstantBlock
				Texture, //Texture
				Namespace, //Namespace
				Comment, // // and /**/
				PipelinePass, // VertexPass()
				Body,
				Member,
				Symbol,
				Parameter,
				Expression,
				Element
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);

			Type type;
			std::vector<Tokenizer::Token> _tokens;
			std::vector<Instruction> nestedInstructions;

			friend std::ostream& operator << (std::ostream& p_os, const Instruction& p_instruction);

			void addNestedInstruction(const Lexer::Instruction& p_instruction);
		};

		struct Result
		{
			std::vector<Instruction> instructions;
			std::vector<CompilationError> errors;
		};

	private:
		std::vector<Tokenizer::Token> _tokens;
		Instruction _instruction;
		Result _result;
		size_t _index;

		bool hasTokenLeft() const;
		const Tokenizer::Token& previousToken();
		const Tokenizer::Token& currentToken() const;
		const Tokenizer::Token& nextToken();

		void skipWord();
		void skipLine();
		void skipComment();
		void insertError(const std::string& p_errorMessage);

		void consume(Instruction& p_instruction);
		void consumeOnSameLine(Instruction& p_instruction);
		void consumeCheck(Tokenizer::Token::Type p_type, const std::string& p_errorMessage);
		void onSameLineCheck(size_t p_line);
		void consume(Instruction& p_instruction, Tokenizer::Token::Type p_type, const std::string& p_errorMessage);
		void consumeOnSameLine(Instruction& p_instruction, Tokenizer::Token::Type p_type, const std::string& p_errorMessage);

		void consumeAndCheckValue(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::string p_expectedValue, const std::string& p_errorMessage);
		void consumeOnSameLineAndCheckValue(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::string p_expectedValue, const std::string& p_errorMessage);

		void consumeAndCheckValues(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::vector<std::string> p_possibleValues, const std::string& p_errorMessage);
		void consumeOnSameLineAndCheckValues(Instruction& p_instruction, Tokenizer::Token::Type p_type, std::vector<std::string> p_possibleValues, const std::string& p_errorMessage);

		void advance();
		void appendInstruction(const Lexer::Instruction& p_instruction);

		Instruction parseExpressionElement();
		Instruction parseExpression();

		Instruction parseMember();

		Instruction parseParameter();
		Instruction parseSymbol();

		Instruction parseFunctionBody();

		void parseBlockBody(Instruction& p_instruction, bool p_canAssign);

		Instruction parseInclude();
		Instruction parsePipelineFlow();
		Instruction parseStructureBlock();
		Instruction parseAttributeBlock();
		Instruction parseConstantBlock();
		Instruction parsePipelineDefinition();
		Instruction parseFunction();
		Instruction parseTexture();

		Instruction parseNamespace();

	public:
		void execute(const std::vector<Tokenizer::Token>& p_tokens);

		const Result& result() const;

		static Result checkGrammar(const std::vector<Tokenizer::Token>& p_tokens);
	};
}

std::string to_string(Lumina::Lexer::Instruction::Type p_type);