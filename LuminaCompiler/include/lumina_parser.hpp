#pragma once

#include "lumina_lexer.hpp"

#include <set>

namespace Lumina
{
	struct Parser
	{
		struct Result
		{
			std::vector<CompilationError> errors;
		};

		static Result checkSemantic(std::vector<Lexer::Element> p_elements);
	};
}