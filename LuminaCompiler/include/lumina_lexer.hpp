#pragma once

#include <fstream>
#include <vector>
#include <memory>
#include <filesystem>
#include "lumina_utils.hpp"
#include "lumina_token.hpp"
#include "lumina_tokenizer.hpp"
#include "lumina_exception.hpp"
#include "lumina_instructions.hpp"

namespace Lumina
{
	class Lexer
	{
	public:
		struct Result
		{
			std::vector<std::shared_ptr<Lumina::Instruction>> instructions;
			std::vector<Lumina::TokenBasedError> errors;
		};

		Lexer() = default;

		static Result checkSyntax(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens);

	private:
		std::filesystem::path _file;
		Result _result;
		Lumina::Token noToken;
		const std::vector<Lumina::Token>* _tokens;
		size_t _index = 0;

		bool hasTokenLeft() const;
		void backOff();
		void advance();
		const Lumina::Token& currentToken() const;
		const Lumina::Token& tokenAtIndex(size_t p_index) const;
		const Lumina::Token& nextToken() const;
		void skipToken();
		void skipLine();
		const Lumina::Token& expect(Lumina::Token::Type p_expectedType, const std::string& p_errorMessage = "Unexpected token type.");
		const Lumina::Token& expect(std::vector<Lumina::Token::Type> p_expectedTypes, const std::string& p_errorMessage = "Unexpected token type.");

		bool describeSymbolCallName();
		bool describeVariableDeclarationInstruction();
		bool describeSymbolCallInstruction();
		bool describeVariableAssignationInstruction();

		std::shared_ptr<Lumina::IncludeInstruction> parseIncludeInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::TypeInstruction> parseTypeInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::IdentifierInstruction> parseIdentifierInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::PipelineFlowInstruction> parsePipelineFlowInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::BlockElementInstruction> parseBlockElementInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::StructureBlockInstruction> parseStructureBlockInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::AttributeBlockInstruction> parseAttributeBlockInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::ConstantBlockInstruction> parseConstantBlockInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::TextureInstruction> parseTextureInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::SymbolParameterInstruction> parseSymbolParameterInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::OperatorExpressionInstruction> parseOperatorExpressionInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::NumberExpressionValueInstruction> parseNumberExpressionValueInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::StringLiteralsExpressionValueInstruction> parseStringLiteralsExpressionValueInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::VariableExpressionValueInstruction> parseVariableExpressionValueInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::SymbolNameInstruction> parseSymbolCallNameInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::SymbolCallInstruction> parseSymbolCallInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::ExpressionInstruction> parseExpression(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::VariableDeclarationInstruction> parseVariableDeclarationInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::ReturnInstruction> parseReturnInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::DiscardInstruction> parseDiscardInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::VariableDesignationInstruction> parseVariableDesignationInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::VariableAssignationInstruction> parseVariableAssignationInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::ConditionInstruction> parseConditionInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::IfStatementInstruction> parseIfStatementInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::WhileLoopInstruction> parseWhileLoopInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::ForLoopInstruction> parseForLoopInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::SymbolBodyInstruction> parseSymbolBodyInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::SymbolInstruction> parseSymbolInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::NamespaceInstruction> parseNamespaceInstruction(const std::string& p_debugInformation = "");
		std::shared_ptr<Lumina::PipelineBodyInstruction> parsePipelineBodyInstruction(const std::string& p_debugInformation = "");

		Result parse(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens);
	};
}