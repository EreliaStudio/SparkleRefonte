#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	std::string SemanticChecker::namespacePrefix() const
	{
		std::string result = "";

		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				result += "::";
			result += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			result += "::";

		return (result);
	}
}