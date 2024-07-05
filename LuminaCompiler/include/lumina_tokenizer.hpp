#pragma once

#include <vector>
#include <string>

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
				NamespaceSeparator,
				Keyword,
				Identifier,
				Number,
				StringLiteral,
				Assignator,
				Operator,
				Separator,
				BodyOpener,
				BodyCloser,
				ParenthesisOpener,
				ParenthesisCloser,
				InstructionEnd,
				Accessor,
				Comment,
				SingleLineComment,
				MultilineComment,
				EndOfMultilineComment,
				EndOfSentence,
				Unknown
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);
			friend std::wostream& operator << (std::wostream& p_os, const Type& p_type);

			Type type = Type::Unknown;
			std::string content = "";
			int line = 0;
			size_t column = 0;

			friend std::ostream& operator << (std::ostream& p_os, const Token& p_token);
		};

	public:
		static std::vector<Token> tokenize(const std::string& p_code);
	};
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type);
std::wstring to_wstring(Lumina::Tokenizer::Token::Type p_type);