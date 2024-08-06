#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkSymbolCallExpression(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& symbolCall)
	{
		std::string symbolName = symbolCall->name->string();

		auto it = _alreadyCreatedSymbols.find(Symbol(symbolName, {}, {}));
		if (it == _alreadyCreatedSymbols.end())
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

			symbolName = namespacePrefix + symbolName;

			it = _alreadyCreatedSymbols.find(Symbol(symbolName, {}, {}));
			if (it == _alreadyCreatedSymbols.end())
			{
				if (_currentNamespace.size() == 0)
				{
					throw TokenBasedError(p_file, "Symbol [" + symbolName + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
				}
				else
				{
					throw TokenBasedError(p_file, "Symbol [" + symbolName + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
				}
			}
		}

		const Symbol& symbol = *it;
		if (symbol.parameters.size() != symbolCall->arguments.size())
		{
			throw TokenBasedError(p_file, "Symbol [" + symbolName + "] called with incorrect number of arguments" + DEBUG_INFORMATION, Lumina::Token::merge(symbolCall->name->tokens, Lumina::Token::Type::Identifier));
		}
	}

	void SemanticChecker::checkExpressionInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
	{

	}

	void SemanticChecker::checkVariableDeclarationInstruction(const std::filesystem::path& p_file, const std::shared_ptr<VariableDeclarationInstruction>& p_instruction, VariableStorage p_scopeVariables)
	{
		std::string variableTypeName = p_instruction->type->string();

		if (_alreadyCreatedStructures.find(variableTypeName) == _alreadyCreatedStructures.end())
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

			variableTypeName = namespacePrefix + variableTypeName;

			if (_alreadyCreatedStructures.find(variableTypeName) == _alreadyCreatedStructures.end())
			{
				if (_currentNamespace.size() == 0)
				{
					throw TokenBasedError(p_file, "Variable type [" + p_instruction->type->string() + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
				}
				else
				{
					throw TokenBasedError(p_file, "Variable type [" + p_instruction->type->string() + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->type->tokens, Lumina::Token::Type::Identifier));
				}
			}
		}

		Variable tmpVariable = Variable(p_instruction->name.content, variableTypeName);

		if (p_scopeVariables.contains(tmpVariable.name) == true)
		{
			throw TokenBasedError(p_file, "Variable named [" + p_instruction->name.content + "] already exist", p_instruction->name);
		}

		p_scopeVariables[tmpVariable.name] = tmpVariable;

		if (p_instruction->initializer != nullptr)
		{
			std::string expressionType = getExpressionType(p_file, p_instruction->initializer, p_scopeVariables);

			if (expressionType != tmpVariable.type)
			{
				throw TokenBasedError(p_file, "Initializer type invalid. Expected a [" + tmpVariable.type.name + "] type but received a [" + expressionType + "]", Lumina::Token::merge(p_instruction->initializer->getTokens(), Lumina::Token::Type::Identifier));
			}

			checkExpressionInstruction(p_file, p_instruction->initializer, p_scopeVariables);
		}
	}

	void SemanticChecker::checkReturnInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ReturnInstruction>& p_instruction, VariableStorage p_scopeVariables, const std::string& p_expectedReturnType)
	{
		std::string returnType = getExpressionType(p_file, p_instruction->argument, p_scopeVariables);

		if (returnType != p_expectedReturnType)
		{
			throw TokenBasedError(p_file, "Unexpected return type [" + returnType + "] when expected [" + p_expectedReturnType + "]", Lumina::Token::merge(p_instruction->argument->getTokens(), Lumina::Token::Type::Identifier));
		}

		checkExpressionInstruction(p_file, p_instruction->argument, p_scopeVariables);
	}
}