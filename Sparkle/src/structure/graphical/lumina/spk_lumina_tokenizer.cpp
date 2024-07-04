#include "structure/graphical/lumina/spk_lumina_tokenizer.hpp"

#include <sstream>
#include <string>

#include <iostream>

namespace spk
{
	namespace Lumina
	{
		void parseLine(const std::string& line, std::vector<Tokenizer::Token>& tokens)
		{
			std::stringstream lineStream(line);
			while ()
			{

			}
		}

		std::vector<Tokenizer::Token> Tokenizer::tokenize(const std::string& p_code)
		{
			std::vector<Tokenizer::Token> tokens;

			std::stringstream codeStream(p_code);

			std::string line;
			while (getline(codeStream, line))
			{
				std::cout << "Line : " << line << std::endl;;

				parseLine(line, tokens);
			}

			return tokens;
		}
	}
}