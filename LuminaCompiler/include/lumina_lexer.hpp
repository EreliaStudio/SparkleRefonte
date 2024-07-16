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

		struct Element
		{
			enum class Type
			{
				Import,
				PipelineFlow,
				PipelineDefinition,
				Structure,
				Attribute,
				Constant,
				BlockElement,
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
				Element,
				VariableDeclaration,
				VariableAssignation,
				Unknow
			};

			Type type = Type::Unknow;
			std::vector<Tokenizer::Token> tokens;
			std::vector<Element> nestedElement;

			void insert(const Tokenizer::Token& p_token);
			void insertNestedElement(const Element& p_instruction);

			friend std::ostream& operator << (std::ostream& p_os, const Element::Type& p_type);
			void print(std::fstream& p_outputStream, size_t p_tabulation = 0, size_t p_startingLine = 0) const;
			void print(size_t p_tabulation = 0, size_t p_startingLine = 0) const;

			std::vector<Tokenizer::Token> tokenList() const;
		};

		struct Result
		{
			std::vector<Element> elements;
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
		void consume(Element& p_instruction);
		void consume(Element& p_instruction, TokenType p_expectedTokenType);
		void consume(Element& p_instruction, TokenType p_expectedTokenType, const std::string& p_errorMessage);
		void consume(Element& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, const std::string& p_errorMessage);
		void consumeMultiple(Element& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, const std::string& p_errorMessage);

		void consumeAndReassign(Element& p_instruction, TokenType p_reassignedTokenType);
		void consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, TokenType p_reassignedTokenType);
		void consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, TokenType p_reassignedTokenType, const std::string& p_errorMessage);
		void consumeAndReassign(Element& p_instruction, TokenType p_expectedTokenType, const std::string& p_expectedTokenContent, TokenType p_reassignedTokenType, const std::string& p_errorMessage);
		void consumeMultipleAndReassign(Element& p_instruction, TokenType p_expectedTokenType, const std::vector<std::string>& p_expectedTokenContents, TokenType p_reassignedTokenType, const std::string& p_errorMessage);


		void skipComment();

		Element parseCallParameters();

		void expendExpression(Element& p_instruction);
		Element parseExpression();

		Element parseIfStatement();
		Element parseWhileStatement();
		Element parseForStatement();

		Element parseFunctionInstruction();
		Element parseReturn();
		Element parseDiscard();

		Element parseBlockElement(bool p_parseAssignator);
		Element parseBlockBody(bool p_parseAssignator);
		Element parseSymbolParameters();
		Element parseOnelinerSymbolBody();
		Element parseSymbolBody();
		Element parseNamespaceBody();

		Element parsePipelineFlow();
		Element parsePipelineDefinition();
		Element parseImport();
		Element parseStructureBlock();
		Element parseAttributeBlock();
		Element parseConstantBlock();
		Element parseTexture();
		Element parseSymbol();
		Element parseNamespace();

	public:
		Lexer();
		Result execute(const std::vector<Tokenizer::Token>& p_tokens);

		static Result checkGrammar(const std::vector<Tokenizer::Token>& p_tokens);
	};
}

std::string to_string(const Lumina::Lexer::Element::Type& p_type);