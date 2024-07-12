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
			enum class Type
			{
				Import,
				PipelineFlow,
				PipelineDefinition,
				Structure,
				Attribute,
				Constant,
				Body,
				Expression,
				CallParameters,
				Texture,
				Symbol,
				SymbolParameters,
				Namespace,
				IfStatement,
				WhileStatement,
				ForStatement,
				Return,
				Discard,
				Instruction,
				VariableDeclaration,
				Unknow
			};

			Type type = Type::Unknow;
			std::vector<Tokenizer::Token> tokens;
			std::vector<Instruction> nestedInstructions;

			void insert(const Tokenizer::Token& p_token);
			void insertNestedInstruction(const Instruction& p_instruction);

			friend std::ostream& operator << (std::ostream& p_os, const Instruction::Type& p_type);
			void print(std::fstream& p_outputStream, size_t p_tabulation = 0, size_t startingIndex = 0) const;
		};

		struct Result
		{
			std::vector<Instruction> instructions;
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
		void consume(Instruction& p_instruction);
		void consume(Instruction& p_instruction, TokenType p_expectedTokenType);
		void consume(Instruction& p_instruction, TokenType p_expectedTokenType, const std::string& p_errorMessage);
		void consume(Instruction& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, const std::string& p_errorMessage);
		void consumeMultiple(Instruction& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, const std::string& p_errorMessage);

		void skipComment();

		Instruction parseCallParameters();

		void expendExpression(Instruction& p_instruction);
		Instruction parseExpression();

		Instruction parseIfStatement();
		Instruction parseWhileStatement();
		Instruction parseForStatement();

		Instruction parseFunctionInstruction();
		Instruction parseReturn();
		Instruction parseDiscard();

		Instruction parseBlockBody(bool p_parseAssignator);
		Instruction parseSymbolParameters();
		Instruction parseOnelinerSymbolBody();
		Instruction parseSymbolBody();
		Instruction parseNamespaceBody();

		Instruction parsePipelineFlow();
		Instruction parsePipelineDefinition();
		Instruction parseImport();
		Instruction parseStructureBlock();
		Instruction parseAttributeBlock();
		Instruction parseConstantBlock();
		Instruction parseTexture();
		Instruction parseSymbol();
		Instruction parseNamespace();

	public:
		Lexer();
		Result execute(const std::vector<Tokenizer::Token>& p_tokens);

		static Result checkGrammar(const std::vector<Tokenizer::Token>& p_tokens);
	};
}

std::string to_string(const Lumina::Lexer::Instruction::Type& p_type);