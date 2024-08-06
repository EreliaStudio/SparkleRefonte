#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolInstruction>& p_instruction)
	{
		std::string namespacePrefix = "";
		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				namespacePrefix += "::";
			namespacePrefix += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			namespacePrefix += "::";

		std::string name = namespacePrefix + p_instruction->name.content;

		std::vector<Structure> parameters;
		for (const auto& param : p_instruction->parameters)
		{
			parameters.push_back(*(_alreadyCreatedStructures.find(param->type->string())));
		}

		Symbol symbolToCheck(name, p_instruction->returnType->string(), parameters);

		if (_alreadyCreatedSymbols.find(symbolToCheck) != _alreadyCreatedSymbols.end())
		{
			if (_currentNamespace.size() == 0)
			{
				throw TokenBasedError(p_file, "Symbol [" + p_instruction->name.content + "] already created" + DEBUG_INFORMATION, p_instruction->name);
			}
			else
			{
				throw TokenBasedError(p_file, "Symbol [" + p_instruction->name.content + "] already created inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, p_instruction->name);
			}
		}

		_alreadyCreatedSymbols.insert(symbolToCheck);

		if (_alreadyCreatedStructures.find(p_instruction->returnType->string()) == _alreadyCreatedStructures.end())
		{
			throw TokenBasedError(
				p_file,
				"Return type [" + p_instruction->returnType->string() + "] not found" + DEBUG_INFORMATION,
				Lumina::Token::merge(p_instruction->returnType->tokens, Lumina::Token::Type::Identifier)
			);
		}

		VariableStorage scopeVariables;

		for (const auto& parameter : p_instruction->parameters)
		{
			if (_alreadyCreatedStructures.find(parameter->type->string()) == _alreadyCreatedStructures.end())
			{
				throw TokenBasedError(
					p_file,
					"Parameter type [" + parameter->type->string() + "] not found" + DEBUG_INFORMATION,
					Lumina::Token::merge(parameter->type->tokens, Lumina::Token::Type::Identifier)
				);
			}

			Variable tmpVariable = Variable(parameter->name.content, *(_alreadyCreatedStructures.find(parameter->type->string())));

			if (scopeVariables.contains(tmpVariable.name) == true)
			{
				throw TokenBasedError(
					p_file,
					"Parameter [" + parameter->name.content + "] already defined" + DEBUG_INFORMATION,
					parameter->name
				);
			}

			scopeVariables[tmpVariable.name] = tmpVariable;
		}

		checkSymbolBodyInstruction(p_file, p_instruction->body, scopeVariables, p_instruction->returnType->string());
	}
}