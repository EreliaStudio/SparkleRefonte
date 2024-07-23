#include <fstream>

#include "lumina_utils.hpp"
#include "lumina_token.hpp"
#include "lumina_tokenizer.hpp"

#include "lumina_exception.hpp"

class Lexer
{
public:
	struct Result
	{
		std::vector<Lumina::TokenBasedError> errors;
	};
	
private:
	std::filesystem::path _file;
	const std::vector<Lumina::Token>* _tokens;
	size_t _index = 0;

	bool hasTokenLeft() const
	{
		return (_index < _tokens->size());
	}
	
	void advance()
	{
		_index++;
	}

	const Lumina::Token& currentToken() const
	{
		return (_tokens->operator[](_index));
	}

	void skipToken()
	{
		_index++;
	}

	void skipLine()
	{
		int currentLine = currentToken().context.line;
		while (hasTokenLeft() == true &&
			currentLine == currentToken().context.line)
		{
			skipToken();
		}
	}

	void expect(Lumina::Token::Type p_expectedType, const std::string& p_errorMessage = "Unexpected token type.")
	{
		if (currentToken().type != p_expectedType)
		{
			throw Lumina::TokenBasedError(_file, p_errorMessage, currentToken());
		}
	}

	Result parse(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens)
	{
		Result result;
		_file = p_file;
		_tokens = &p_tokens;
		_index = 6;

		while (hasTokenLeft() == true)
		{
			result.errors.push_back(Lumina::TokenBasedError(_file, "p_errorMessage", currentToken()));
			skipLine();
		}

		return (result);
	}

public:
	Lexer() = default;

	static Result checkSyntax(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens)
	{
		return (Lexer().parse(p_file, p_tokens));
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

	std::vector<Lumina::Token> tokens = Lumina::Tokenizer::tokenize(rawInput);

	std::fstream ouputStream;

	ouputStream.open("resultToken.txt", std::ios_base::out);

	for (const auto& token : tokens)
	{
		ouputStream << token << std::endl;
	}

	ouputStream.close();

	Lexer::Result lexerResult = Lexer::checkSyntax(argv[1], tokens);

	for (const auto& error : lexerResult.errors)
	{
		std::cout << error.what() << std::endl;
	}

	return (0);
}