#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	std::string SemanticChecker::getConstantValueType(const std::filesystem::path& p_file, const std::shared_ptr<NumberExpressionValueInstruction>& p_instruction)
	{
		const std::string& tokenContent = p_instruction->token.content;

		if (tokenContent.find('.') != std::string::npos)
		{
			return ("float");
		}

		if (tokenContent.size() > 2 && (tokenContent.substr(0, 2) == "0x" || tokenContent.substr(0, 2) == "0X"))
		{
			return ("uint");
		}

		if (!tokenContent.empty() && (tokenContent.back() == 'u' || tokenContent.back() == 'U'))
		{
			return ("uint");
		}

		if (tokenContent == "true" || tokenContent == "false")
		{
			return ("bool");
		}

		return ("int");
	}

	std::string SemanticChecker::getVariableType(const std::filesystem::path& p_file, const std::shared_ptr<VariableExpressionValueInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
	{
		std::string variableName = p_instruction->tokens[0].content;

		if (p_scopeVariables.contains(variableName))
		{
			return (p_scopeVariables.at(variableName).type.name);
		}

		return ("float");
	}

	std::string SemanticChecker::getSymbolCallType(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
	{
		std::string symbolName = p_instruction->name->string();

		std::string namespacePrefix = "";
		for (size_t i = 0; i < _currentNamespace.size(); i++)
		{
			if (i != 0)
				namespacePrefix += "::";
			namespacePrefix += _currentNamespace[i].content;
		}
		if (_currentNamespace.size() != 0)
			namespacePrefix += "::";

		if (_alreadyCreatedSymbols.contains(symbolName) == false)
		{
			if (_alreadyCreatedSymbols.contains(namespacePrefix + symbolName) == false)
			{
				if (_currentNamespace.size() == 0)
				{
					throw TokenBasedError(p_file, "Symbol type [" + p_instruction->name->string() + "] not found" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->name->tokens, Lumina::Token::Type::Identifier));
				}
				else
				{
					throw TokenBasedError(p_file, "Variable type [" + p_instruction->name->string() + "] not found inside " + namespacePrefix.substr(0, namespacePrefix.size() - 2) + " namespace" + DEBUG_INFORMATION, Lumina::Token::merge(p_instruction->name->tokens, Lumina::Token::Type::Identifier));
				}
			}
		}

		return (_alreadyCreatedSymbols.find(symbolName)->returnType.name);
	}

	std::string SemanticChecker::getExpressionValueType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionElementInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
	{
		switch (p_instruction->type)
		{
		case Instruction::Type::NumberExpressionValue:
			return (getConstantValueType(p_file, static_pointer_cast<NumberExpressionValueInstruction>(p_instruction)));
		case Instruction::Type::StringLiteralsExpressionValue:
			return ("string");
		case Instruction::Type::VariableExpressionValue:
			return (getVariableType(p_file, static_pointer_cast<VariableExpressionValueInstruction>(p_instruction), p_scopeVariables));
		case Instruction::Type::SymbolCall:
			return (getSymbolCallType(p_file, static_pointer_cast<SymbolCallInstruction>(p_instruction), p_scopeVariables));
		}
		return ("void");
	}

	std::string SemanticChecker::getExpressionType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const VariableStorage& p_scopeVariables)
	{
		const Structure* result = nullptr;

		for (const auto& element : p_instruction->elements)
		{
			if (element->type == Instruction::Type::OperatorExpression)
			{
				if (result != nullptr)
				{

				}
			}
			else
			{
				std::string elementType = getExpressionValueType(p_file, element, p_scopeVariables);

				if (result == nullptr)
				{
					result = &(*(_alreadyCreatedStructures.find(elementType)));
				}
				else if (elementType != result->name)
				{
					std::cout << "Comparing type [" << elementType << "] vs [" << result->name << "]" << std::endl;
					auto it = _alreadyCreatedStructures.find(elementType);
					if (it != _alreadyCreatedStructures.end())
					{
						const Structure& tmpStructure = *it;

						if (std::find(tmpStructure.acceptedConvertions.begin(), tmpStructure.acceptedConvertions.end(), result->name) == tmpStructure.acceptedConvertions.end() ||
							std::find(result->acceptedConvertions.begin(), result->acceptedConvertions.end(), elementType) == result->acceptedConvertions.end())
						{
							throw TokenBasedError(p_file, "Invalid convertion from [" + result->name + "] to [" + elementType + "]" + DEBUG_INFORMATION, Lumina::Token::merge(element->getTokens(), Lumina::Token::Type::Identifier));
						}

						if (it->attributes.size() > result->attributes.size())
						{
							result = &(*(it));
						}
					}
					else
					{
						throw TokenBasedError(p_file, "Type invalid : [" + elementType + "]" + DEBUG_INFORMATION, Lumina::Token::merge(element->getTokens(), Lumina::Token::Type::Identifier));
					}
				}
				std::cout << "Element [" << element->string() << "] type : " << elementType << std::endl;
			}
		}

		if (result == nullptr)
			return ("void");
		return (result->name);
	}
}