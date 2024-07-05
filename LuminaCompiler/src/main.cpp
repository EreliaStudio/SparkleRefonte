#include <iostream>
#include "lumina_tokenizer.hpp"
#include "lumina_lexer.hpp"
#include "lumina_utils.hpp"

using namespace Lumina;

void printInstruction(const Lexer::Instruction& p_instruction, size_t tabulation)
{
	std::cout << std::string(tabulation, ' ') << "Type [" << p_instruction.type << "] - ";
	
	for (size_t i = 0; i < p_instruction.tokens.size(); i++)
	{
		if (i != 0)
			std::cout << " ";
		if (p_instruction.tokens[i].type == Tokenizer::Token::Type::MetaToken)
		{
			std::cout << std::endl;
			printInstruction(p_instruction.nestedInstruction[p_instruction.tokens[i].line], tabulation + 4);
		}
		else
			std::cout << p_instruction.tokens[i].content;
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

	/*size_t tokenID = 0;
	for (const auto& token : tokens)
	{
		std::cout << std::setw(4) << tokenID << " - " << token << std::endl;
		tokenID++;
	}*/

	Lexer::Result grammarResult = Lexer::checkGrammar(tokens);

	if (grammarResult.errors.empty() == false)
	{
		for (const auto& error : grammarResult.errors)
		{
			std::cout << error.what() << std::endl;
			std::cout << std::endl;
		}
	}

	for (const auto& instruction : grammarResult.instructions)
	{
		printInstruction(instruction, 0);
	}

	return (0);
}