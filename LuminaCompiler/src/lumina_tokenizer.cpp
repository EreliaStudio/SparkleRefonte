#include "lumina_tokenizer.hpp"

#include <regex>

namespace Lumina
{
	std::vector<Token> Tokenizer::tokenize(const std::string& p_rawCode)
	{
		std::vector<Token> result;

		// Updated regex pattern to include floating point suffix 'f' for numbers
		std::regex tokenRegex(R"((#include)|(\"(?:[^\"]|\\\")*\")|(<[^ >]+>)|(\b(Input|VertexPass|FragmentPass)\b)|(->)|(::)|(:)|(\bstruct\b)|(\bAttributeBlock\b)|(\bConstantBlock\b)|(\bTexture\b)|(\bnamespace\b)|(\bif\b)|(\bwhile\b)|(\bfor\b)|([a-zA-Z_][a-zA-Z_0-9]*)|([0-9]*\.?[0-9]f)|([0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?|0[xX][0-9a-fA-F]+f?)|(\{)|(\})|(\()|(\))|(\.)|(//.*?$)|(/\*[\s\S]*?\*/)|(==|!=|<=|>=|\|\||&&|[+\-*/%<>!&|^]=?|[~?])|(\breturn\b)|(\bdiscard\b)|(;)|(=)|(\,))");
		std::smatch match;

		auto searchStart = p_rawCode.cbegin();
		int lineNumber = 1;
		int columnNumber = 0;
		std::string currentLine;

		while (std::regex_search(searchStart, p_rawCode.cend(), match, tokenRegex))
		{
			std::string tokenStr = match[0];
			int tokenStartPos = match.position(0);

			Token::Type tokenType = Token::Type::Unknow;
			if (match[1].matched) tokenType = Token::Type::Include;
			else if (match[2].matched) tokenType = Token::Type::StringLitteral;
			else if (match[3].matched) tokenType = Token::Type::IncludeLitteral;
			else if (match[4].matched || match[5].matched) tokenType = Token::Type::PipelineFlow;
			else if (match[6].matched) tokenType = Token::Type::PipelineFlowSeparator;
			else if (match[7].matched) tokenType = Token::Type::NamespaceSeparator;
			else if (match[8].matched) tokenType = Token::Type::Separator;
			else if (match[9].matched) tokenType = Token::Type::StructureBlock;
			else if (match[10].matched) tokenType = Token::Type::AttributeBlock;
			else if (match[11].matched) tokenType = Token::Type::ConstantBlock;
			else if (match[12].matched) tokenType = Token::Type::Texture;
			else if (match[13].matched) tokenType = Token::Type::Namespace;
			else if (match[14].matched) tokenType = Token::Type::IfStatement;
			else if (match[15].matched) tokenType = Token::Type::WhileStatement;
			else if (match[16].matched) tokenType = Token::Type::ForStatement;
			else if (match[17].matched) tokenType = Token::Type::Identifier;
			else if (match[18].matched || match[19].matched || match[20].matched) tokenType = Token::Type::Number;
			else if (match[21].matched) tokenType = Token::Type::OpenCurlyBracket;
			else if (match[22].matched) tokenType = Token::Type::CloseCurlyBracket;
			else if (match[23].matched) tokenType = Token::Type::OpenParenthesis;
			else if (match[24].matched) tokenType = Token::Type::CloseParenthesis;
			else if (match[25].matched) tokenType = Token::Type::Accessor;
			else if (match[26].matched || match[27].matched) tokenType = Token::Type::Comment;
			else if (match[28].matched) tokenType = Token::Type::Operator;
			else if (match[29].matched) tokenType = Token::Type::Return;
			else if (match[30].matched) tokenType = Token::Type::Discard;
			else if (match[31].matched) tokenType = Token::Type::EndOfSentence;
			else if (match[32].matched) tokenType = Token::Type::Assignator;
			else if (match[33].matched) tokenType = Token::Type::Comma;

			// Calculate the line and column numbers
			auto prefix = std::string(searchStart, match[0].first);
			for (char ch : prefix)
			{
				if (ch == '\n')
				{
					lineNumber++;
					columnNumber = 0;
					currentLine.clear();
				}
				else
				{
					columnNumber++;
					currentLine += ch;
				}
			}

			// Extract the input line containing the token
			if (currentLine.empty())
			{
				size_t lineStartPos = p_rawCode.rfind('\n', searchStart - p_rawCode.cbegin() + tokenStartPos);
				size_t lineEndPos = p_rawCode.find('\n', searchStart - p_rawCode.cbegin() + tokenStartPos + tokenStr.size());
				currentLine = p_rawCode.substr(lineStartPos + 1, lineEndPos - lineStartPos - 1);
			}

			result.emplace_back(tokenStr, tokenType, lineNumber, columnNumber, currentLine);

			// Update searchStart to the end of the matched token
			searchStart = match.suffix().first;

			// Adjust line and column numbers for tokens that contain newlines
			for (char ch : tokenStr)
			{
				if (ch == '\n')
				{
					lineNumber++;
					columnNumber = 0;
					currentLine.clear();
				}
				else
				{
					columnNumber++;
					currentLine += ch;
				}
			}
		}

		return result;
	}
}