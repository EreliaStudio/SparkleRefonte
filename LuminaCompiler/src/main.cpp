#include <fstream>

#include "lumina_utils.hpp"
#include "lumina_token.hpp"
#include "lumina_tokenizer.hpp"
#include "lumina_lexer.hpp"


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

	Lumina::Lexer::Result lexerResult = Lumina::Lexer::checkSyntax(argv[1], tokens);

	for (const auto& error : lexerResult.errors)
	{
		std::cout << error.what() << std::endl;
	}

	return (0);
}