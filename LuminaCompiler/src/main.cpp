#include <iostream>
#include "lumina_tokenizer.hpp"
#include "lumina_utils.hpp"

using namespace Lumina;

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	std::vector<Tokenizer::Token> tokens = Tokenizer::tokenize(readFileAsString(argv[1]));

	return (0);
}