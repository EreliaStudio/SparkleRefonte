#include <iostream>
#include "lumina_tokenizer.hpp"
#include "lumina_lexer.hpp"
#include "lumina_utils.hpp"

#include <fstream>

using namespace Lumina;


int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	std::vector<Tokenizer::Token> tokens = Tokenizer::tokenize(readFileAsString(argv[1]));

	std::fstream ouputTokenFile;

	ouputTokenFile.open("resultToken.txt", std::ios_base::out);

	size_t tokenID = 0;
	for (const auto& token : tokens)
	{
		ouputTokenFile << std::setw(4) << tokenID << " - " << token << std::endl;
		tokenID++;
	}
	ouputTokenFile.close();

	Lexer::Result lexerResult = Lexer::checkGrammar(tokens);

	for (const auto& error : lexerResult.errors)
	{
		std::cout << error.what() << std::endl << std::endl;
	}

	std::fstream outputInstructionFile;

	outputInstructionFile.open("resultInstruction.txt", std::ios_base::out);

	for (const auto& instruction : lexerResult.instructions)
	{
		instruction.print(outputInstructionFile);
		outputInstructionFile << std::endl;
	}
	outputInstructionFile.close();

	return (0);
}