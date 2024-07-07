#pragma once

#include <vector>
#include <string>
#include <iostream>

namespace Lumina
{
	class Tokenizer
	{
	public:
		struct Token
		{
			enum class Type
			{
				Include,
				PipelineFlow,
				PipelineFlowSeparator,
				Keyword,
				Identifier,
				Number,
				StringLiteral,
				Assignator,
				Operator,
				Separator,
				Comma,
				NamespaceSeparator,
				BodyOpener,
				BodyCloser,
				ParenthesisOpener,
				ParenthesisCloser,
				InstructionEnd,
				Accessor,
				SingleLineComment,
				MultilineComment,
				EndOfMultilineComment,
				EndOfSentence,
				Unknown,
				MetaToken
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);

			Type type = Type::Unknown;
			std::string content = "";
			size_t line = 0;
			size_t column = 0;
			std::string fullLine = "";

			friend std::ostream& operator << (std::ostream& p_os, const Token& p_token);

			static Token createMetaToken(size_t metaTokenID);
		};

	public:
		static std::vector<Token> tokenize(const std::string& p_code);
	};
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type);