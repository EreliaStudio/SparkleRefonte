#include <iostream>
#include "lumina_tokenizer.hpp"
#include "lumina_lexer.hpp"
#include "lumina_utils.hpp"

#include <fstream>

using namespace Lumina;

void printInstruction(const Lexer::Instruction& p_instruction, size_t tabulation)
{
	std::cout << std::string(tabulation, ' ') << "Type [" << p_instruction.type << "] - ";
	
	for (size_t i = 0; i < p_instruction._tokens.size(); i++)
	{
		if (i != 0)
			std::cout << " ";
		if (p_instruction._tokens[i].type == Tokenizer::Token::Type::MetaToken)
		{
			std::cout << std::endl;
			printInstruction(p_instruction.nestedInstructions[p_instruction._tokens[i].line], tabulation + 4);
		}
		else
			std::cout << p_instruction._tokens[i].content;
	}
	std::cout << std::endl;
}

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

	Lexer::Result grammarResult = Lexer::checkGrammar(tokens);

	for (const auto& instruction : grammarResult.instructions)
	{
		printInstruction(instruction, 0);
	}

	if (grammarResult.errors.empty() == false)
	{
		for (const auto& error : grammarResult.errors)
		{
			std::cout << error.what() << std::endl;
			std::cout << std::endl;
		}
	}

	return (0);
}