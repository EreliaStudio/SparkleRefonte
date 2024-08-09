#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolInstruction>& p_instruction)
	{
		Symbol newSymbol;

		auto returnTypeToken = Token::merge(p_instruction->returnType->tokens, Token::Type::Identifier);

		newSymbol.returnType = type(returnTypeToken.content);
		if (newSymbol.returnType == nullptr)
		{
			throwException(p_file, "Return type [" + returnTypeToken.content + "] not found", returnTypeToken);
		}

		newSymbol.name = createNamespacePrefix() + p_instruction->name.content;
		for (const auto& parameter : p_instruction->parameters)
		{
			auto parameterTypeToken = Token::merge(parameter->type->tokens, Token::Type::Identifier);
			Type* parameterType = type(parameterTypeToken.content);

			if (parameterType == nullptr)
			{
				throwException(p_file, "Parameter type [" + parameterTypeToken.content + "] not found", returnTypeToken);
			}

			newSymbol.parameters.push_back(parameterType);
		}

		const std::vector<Symbol>& tmpSymbolArray = symbolArray(newSymbol.name);

		if (tmpSymbolArray.size() != 0 &&
			tmpSymbolArray.back().returnType != newSymbol.returnType)
		{
			throw TokenBasedError(p_file, "Symbol [" + returnTypeToken.content + "] already define with another return type.", returnTypeToken);
		}

		for (const auto& symbol : tmpSymbolArray)
		{
			if (symbol.parameters.size() == newSymbol.parameters.size())
			{
				bool isSame = true;

				for (size_t i = 0; isSame == true && i < symbol.parameters.size(); i++)
				{
					if (symbol.parameters[i] != newSymbol.parameters[i])
					{
						isSame = false;
					}
				}

				if (isSame == true)
				{
					std::vector<Token> tokens = { returnTypeToken, p_instruction->parameters.back()->name };
					tokens.back().content += ")";
					Token mergedToken = Token::merge(tokens, Token::Type::Identifier);

					throw TokenBasedError(p_file, "Symbol [" + mergedToken.content + "] already define with the same parameter type.", mergedToken);
				}
			}
		}

		addSymbol(newSymbol);

		std::unordered_map<std::string, Type*> functionVariables;

		for (const auto attribute : _attributes)
		{
			functionVariables[attribute->name] = attribute;
		}

		for (const auto constant : _constants)
		{
			functionVariables[constant->name] = constant;
		}

		checkSymbolBodyInstruction(p_file, p_instruction->body, functionVariables);
	}
}