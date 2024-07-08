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
				SingleLineComment,
				MultiLineCommentStart,
				MultiLineCommentStop,
				Include,
				DoubleQuote,
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
				OpenedParenthesis,
				ClosedParenthesis,
				Comma,
				Operator,
				EndOfSentence,
				Number,
				Namespace,
				BodyOpener,
				BodyCloser,
				IfStatement,
				WhileStatement,
				ForStatement,
				Unknown
			};

			friend std::ostream& operator << (std::ostream& p_os, const Type& p_type);

			Type type = Type::Unknown;
			std::string content = "";
			size_t line = 0;
			size_t column = 0;
			std::string fullLine = "";

			friend std::ostream& operator << (std::ostream& p_os, const Token& p_token);
		};

		static void generateTokens(std::stringstream p_codeStream, std::vector<Token>& p_tokens);
		static void mergeTokens(std::vector<Token>& p_tokens);
		static void assignTokensType(std::vector<Token>& p_tokens);

	public:
		static std::vector<Token> tokenize(const std::string& p_code);
	};
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type);