#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <filesystem>

namespace Lumina
{
	class Tokenizer
	{
	public:
		struct Token
		{
			enum class Type
			{
				SingleLineComment,
				MultiLineCommentStart,
				MultiLineCommentStop,
				Import,
				StringLitterals,
				PipelineFlow,
				PipelineSeparator,
				Separator,
				Structure,
				Identifier,
				Texture,
				AttributeBlock,
				ConstantBlock,
				Assignator,
				Accessor,
				Symbol,
				OpenParenthesis,
				ClosedParenthesis,
				Comma,
				Operator,
				EndOfSentence,
				Number,
				Bool,
				Namespace,
				NamespaceSeparator,
				OpenCurlyBracket,
				ClosedCurlyBracket,
				IfStatement,
				WhileStatement,
				ForStatement,
				SwitchStatement,
				Case,
				Break,
				Return,
				Discard,
				Unknown,
				MetaToken,
				Type,
				Name,
				SymbolCall
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);

			Type type = Type::Unknown;
			std::string content = "";
			size_t line = 0;
			size_t column = 0;
			std::string fullLine = "";
			std::string fileName = "";

			friend std::ostream& operator << (std::ostream& p_os, const Token& p_token);
		};

		static void generateTokens(std::stringstream p_codeStream, std::vector<Token>& p_tokens);
		static void mergeTokens(std::vector<Token>& p_tokens);
		static void assignTokensType(std::vector<Token>& p_tokens);

	public:
		static std::vector<Token> tokenize(const std::filesystem::path& p_filePath);
	};
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type);