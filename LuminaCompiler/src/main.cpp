#include <iostream>
#include <iomanip>

#include "lumina_utils.hpp"

struct Token
{
	struct Position
	{
		size_t line;
		size_t column;
	};

	enum class Type
	{
		Unknow
	};

	Type type;

	std::string content;
	
	Position position;

	friend std::ostream& operator << (std::ostream& p_os, const Token::Type& p_type)
	{
		switch (p_type)
		{
		case Token::Type::Unknow:
			p_os << "Unknow";
		}

		return (p_os);
	}

	friend std::ostream& operator << (std::ostream& p_os, const Token& p_token)
	{
		p_os << std::setw(10) << p_token.type << " - " << p_token.content;

		return (p_os);
	}	
};

struct Tokenizer
{
	static std::vector<Token> generateRawToken(const std::string& p_rawCode)
	{
		std::vector<Token> result;

		return (result);
	}

	static std::vector<Token> mergeTokens(const std::vector<Token>& p_rawTokens)
	{
		std::vector<Token> result;

		return (result);
	}

	static void defineTokensType(std::vector<Token>& p_tokens)
	{
		
	}

	static std::vector<Token> tokenize(const std::string& p_rawCode)
	{
		std::vector<Token> rawTokens = generateRawToken(p_rawCode);
		std::vector<Token> mergedTokens = mergeTokens(rawTokens);
		defineTokensType(mergedTokens);

		return (mergedTokens);
	}
};

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	std::string rawInput = Lumina::readFileAsString(argv[1]);

	std::vector<Token> tokens = Tokenizer::tokenize(rawInput);

	for (const auto& token : tokens)
	{
		std::cout << token << std::endl;
	}

	return (0);
}