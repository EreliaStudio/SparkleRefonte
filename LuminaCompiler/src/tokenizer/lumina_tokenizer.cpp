#include "lumina_tokenizer.hpp"

#include <sstream>
#include <string>

#include <iostream>

#include <unordered_set>
#include <unordered_map>

#include <algorithm>

#define DEBUG_LINE() std::cout << __FUNCTION__ << "::" << __LINE__ << std::endl

namespace Lumina
{
	bool is_alpha(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](unsigned char c) { return !std::isalpha(c); }) == s.end();
	}

	bool is_hex(const std::string& s)
	{
		if (s.size() < 3 || s[0] != '0' || s[1] != 'x')
		{
			return false;
		}

		for (auto it = s.begin() + 2; it != s.end(); ++it)
		{
			if (!std::isdigit(*it) && !(*it >= 'a' && *it <= 'f') && !(*it >= 'A' && *it <= 'F'))
			{
				return false;
			}
		}

		return true;
	}

	bool is_binary(const std::string& s)
	{
		if (s.size() < 3 || s[0] != '0' || s[1] != 'b')
		{
			return false;
		}

		for (auto it = s.begin() + 2; it != s.end(); ++it)
		{
			if (*it != '0' && *it != '1')
			{
				return false;
			}
		}

		return true;
	}

	bool is_number(const std::string& s)
	{
		if (s.empty())
		{
			return false;
		}

		bool decimal_point_seen = false;
		bool digit_seen = false;
		auto it = s.begin();

		// Handle optional sign
		if (*it == '+' || *it == '-')
		{
			++it;
			if (it == s.end())
			{
				return false; // String was just a sign
			}
		}

		for (; it != s.end(); ++it)
		{
			if (std::isdigit(*it))
			{
				digit_seen = true;
			}
			else if (*it == '.')
			{
				if (decimal_point_seen)
				{
					return false; // Multiple decimal points
				}
				decimal_point_seen = true;
			}
			else
			{
				return false; // Invalid character
			}
		}

		return digit_seen;
	}

	bool is_alnum(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(),
			s.end(), [](unsigned char c) { return !std::isalnum(c); }) == s.end();
	}

	bool is_identifier(const std::string& s)
	{
		return !s.empty() && std::find_if(s.begin(), s.end(),
			[](unsigned char c) { return !std::isalnum(c) && c != '_'; }) == s.end();
	}

	void skipSpace(std::stringstream& lineStream)
	{
		while (lineStream.peek() != EOF && isspace(lineStream.peek()))
		{
			char ch;

			lineStream.get(ch);
		}
	}

	static bool isParsingStringLitteral = false;

	bool getword(std::stringstream& lineStream, std::string& word)
	{
        word.clear();

		if (lineStream.peek() == EOF)
			return false;

		if (isParsingStringLitteral == true)
		{
			char ch;

			while (lineStream.get(ch))
			{
				word += ch;
				if (ch == '\"')
				{
					DEBUG_LINE();
					return (true);
				}
			}
			return (true);
		}

		skipSpace(lineStream);

        char ch;

        while (lineStream.get(ch))
        {
			if (ch == ' ')
			{
				lineStream.unget();
				return (true);
			}
			else if (word.empty() == false && isalnum(ch) == 0 && ch != '_')
			{
				if ((ch != '.' && ch != 'x' && ch != 'b') ||
					(ch == '.' && is_number(word) == false) ||
					(ch == 'x' && is_hex(word) == false) ||
					(ch == 'b' && is_binary(word) == false))
				{
					lineStream.unget();
					return true;
				}
			}
			else if (isalnum(ch) == 0 && ch != '_')
			{
				word += ch;
				return (true);
			}
            word += ch;
        }

        return !word.empty();
	}

	void parseLine(const std::string& line, std::vector<Tokenizer::Token>& tokens, size_t lineNumber)
	{
		std::stringstream lineStream(line);

		static Tokenizer::Token stringLitteralToken;

		std::string word;
        while (getword(lineStream, word))
        {
			if (isParsingStringLitteral == true)
			{
				if (stringLitteralToken.line != lineNumber)
					stringLitteralToken.content += '\n';
				stringLitteralToken.content += word;

				if (stringLitteralToken.content[stringLitteralToken.content.size() - 2] != '\\' &&
					stringLitteralToken.content[stringLitteralToken.content.size() - 1] == '\"')
				{
					isParsingStringLitteral = false;
					tokens.push_back(stringLitteralToken);
				}
			}
			else
			{
				if (word == "\"")
				{
					isParsingStringLitteral = true;
					
					stringLitteralToken.content = word;
					stringLitteralToken.line = lineNumber;
					stringLitteralToken.fullLine = line;
					if (lineStream.peek() == EOF)
						stringLitteralToken.column = line.size() - word.length();
					else
						stringLitteralToken.column = static_cast<size_t>(lineStream.tellg()) - word.length();
					continue;
				}
				Lumina::Tokenizer::Token token;

				token.content = word;
				token.line = lineNumber;
				token.fullLine = line;
				if (lineStream.peek() == EOF)
					token.column = line.size() - word.length();
				else
					token.column = static_cast<size_t>(lineStream.tellg()) - word.length();

				token.type = Tokenizer::Token::Type::Unknown;

				tokens.push_back(token);
			}
        }
	}

	void Tokenizer::generateTokens(std::stringstream p_codeStream, std::vector<Tokenizer::Token>& p_tokens)
	{
		std::string line;
		size_t lineNumber = 0;
		while (getline(p_codeStream, line))
		{
			parseLine(line, p_tokens, lineNumber);
			lineNumber++;
		}
	}

	void Tokenizer::mergeTokens(std::vector<Tokenizer::Token>& p_tokens)
	{
		std::vector<std::string> mergedValue = {
			"//", "/*", "*/", "::", "<=", ">=", "&&", "||", "->", "++", "--"
		};

		// Create a map to quickly check if a combination is in the mergedValue list
		std::unordered_set<std::string> mergedSet(mergedValue.begin(), mergedValue.end());

		std::vector<Tokenizer::Token> mergedTokens;
		size_t i = 0;

		while (i < p_tokens.size())
		{
			std::string combinedContent = p_tokens[i].content;
			size_t j = i + 1;

			// Check if the next token forms a merged value with the current token
			while (j < p_tokens.size() && mergedSet.find(combinedContent + p_tokens[j].content) != mergedSet.end())
			{
				combinedContent += p_tokens[j].content;
				j++;
			}

			// If we found a combination, merge the tokens
			if (combinedContent != p_tokens[i].content)
			{
				Tokenizer::Token mergedToken = p_tokens[i];
				mergedToken.content = combinedContent;

				// Adjust the column to the starting column of the first token in the sequence
				mergedToken.column = p_tokens[i].column;

				mergedTokens.push_back(mergedToken);
				i = j; // Skip the merged tokens
			}
			else
			{
				mergedTokens.push_back(p_tokens[i]);
				i++;
			}
		}

		p_tokens = std::move(mergedTokens);
	}

	void Tokenizer::assignTokensType(std::vector<Tokenizer::Token>& p_tokens)
	{
		std::unordered_map<std::string, Tokenizer::Token::Type> tokenTypeMap = {
			{"//", Token::Type::SingleLineComment},
			{"/*", Token::Type::MultiLineCommentStart},
			{"*/", Token::Type::MultiLineCommentStop},
			{"import", Token::Type::Import},
			{"Input", Token::Type::PipelineFlow},
			{"VertexPass", Token::Type::PipelineFlow},
			{"FragmentPass", Token::Type::PipelineFlow},
			{"->", Token::Type::PipelineSeparator},
			{":", Token::Type::Separator},
			{"::", Token::Type::Namespace},
			{"(", Token::Type::OpenParenthesis},
			{")", Token::Type::ClosedParenthesis},
			{",", Token::Type::Comma},
			{";", Token::Type::EndOfSentence},
			{"{", Token::Type::OpenCurlyBracket},
			{"}", Token::Type::ClosedCurlyBracket},
			{"if", Token::Type::IfStatement},
			{"while", Token::Type::WhileStatement},
			{"for", Token::Type::ForStatement},
			{"=", Token::Type::Assignator},
			{".", Token::Type::Accessor},
			{"<", Token::Type::Operator},
			{">", Token::Type::Operator},
			{"<=", Token::Type::Operator},
			{">=", Token::Type::Operator},
			{"&&", Token::Type::Operator},
			{"||", Token::Type::Operator},
			{"*", Token::Type::Operator},
			{"+", Token::Type::Operator},
			{"++", Token::Type::Operator},
			{"-", Token::Type::Operator},
			{"--", Token::Type::Operator},
			{"/", Token::Type::Operator},
			{"%", Token::Type::Operator},
			{"struct", Token::Type::Structure},
			{"AttributeBlock", Token::Type::AttributeBlock},
			{"ConstantBlock", Token::Type::ConstantBlock},
			{"Texture", Token::Type::Texture},
			{"namespace", Token::Type::Namespace},
			{"switch", Token::Type::SwitchStatement},
			{"case", Token::Type::Case},
			{"break", Token::Type::Break},
			{"return", Token::Type::Return}
		};

		for (auto& token : p_tokens)
		{
			// Check if the token content matches any predefined type
			auto it = tokenTypeMap.find(token.content);
			if (it != tokenTypeMap.end())
			{
				token.type = it->second;
				continue;
			}

			// Check if the token is a number
			if (is_number(token.content) ||
				is_hex(token.content) ||
				is_binary(token.content))
			{
				token.type = Token::Type::Number;
				continue;
			}

			if (token.content.front() == '\"' && token.content.back() == '\"')
			{
				token.type = Token::Type::StringLitterals;
				continue;
			}

			// Check if the token is an identifier (name)
			if (is_identifier(token.content))
			{
				token.type = Token::Type::Identifier;
				continue;
			}

			// Default to unknown if no type is matched
			token.type = Token::Type::Unknown;
		}

	}

	std::vector<Tokenizer::Token> Tokenizer::tokenize(const std::string& p_code)
	{
		std::vector<Tokenizer::Token> tokens;

		generateTokens(std::stringstream(p_code), tokens);
		mergeTokens(tokens);
		assignTokensType(tokens);

		return tokens;
	}
}