#include "lumina_lexer.hpp"

namespace Lumina
{
	Lexer::Result Lexer::execute(const std::vector<Tokenizer::Token>& p_tokens)
	{
		_result = Result();
		_tokens = p_tokens;
		_index = 0;

		for (const auto& token : p_tokens)
		{

		}

		return (_result);
	}
}