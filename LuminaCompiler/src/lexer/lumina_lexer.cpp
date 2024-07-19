#include "lumina_lexer.hpp"

#include "lumina_utils.hpp"

#include <iomanip>

namespace Lumina
{
	Lexer::Result Lexer::execute(const std::vector<Tokenizer::Token>& p_tokens)
	{
		_result = Result();
		_tokens = p_tokens;
		_index = 0;

		while (hasTokenLeft() == true)
		{
			try
			{
				skipLine();
			}
			catch (std::runtime_error& e)
			{
				insertError(e.what());
				skipLine();
			}
		}
		return (_result);
	}
}