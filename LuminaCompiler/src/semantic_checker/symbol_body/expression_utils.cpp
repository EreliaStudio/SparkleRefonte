#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	SemanticChecker::Type* SemanticChecker::getNumberExpressionElementType(const std::filesystem::path& p_file, const std::shared_ptr<NumberExpressionValueInstruction>& p_instruction)
	{
		return (type("float"));
	}

	SemanticChecker::Type* SemanticChecker::getVariableExpressionElementType(const std::filesystem::path& p_file, const std::shared_ptr<VariableExpressionValueInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables)
	{
		if (p_variables.contains(p_instruction->tokens[0].content) == false)
		{
			throw TokenBasedError(p_file, "Variable [" + p_instruction->tokens[0].content + "] Doesn't exist." + DEBUG_INFORMATION, p_instruction->tokens[0]);
		}

		Type* result = p_variables.at(p_instruction->tokens[0].content);

		for (size_t i = 1; i < p_instruction->tokens.size(); i++)
		{
			std::string expectedAttributeName = p_instruction->tokens[i].content;
			const auto& it = std::find_if(result->attributes.begin(), result->attributes.end(), [expectedAttributeName](const Type::Attribute& p_attribute) {
				return (p_attribute.name == expectedAttributeName);
				});

			if (it == result->attributes.end())
			{
				throw TokenBasedError(p_file, "Attribute [" + expectedAttributeName + "] not found in [" + result->name + "] type" + DEBUG_INFORMATION, p_instruction->tokens[i]);
			}

			result = it->type;
		}

		return (result);
	}

	SemanticChecker::Type* SemanticChecker::getSymbolCallExpressionElementType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionElementInstruction>& p_instruction)
	{
		std::shared_ptr<SymbolCallInstruction> value = static_pointer_cast<SymbolCallInstruction>(p_instruction);

		Token nameToken = Token::merge(value->name->tokens, Token::Type::Identifier);
		std::vector<Symbol>* targetSymbolArray = symbolArray(nameToken.content);

		if (targetSymbolArray == nullptr || targetSymbolArray->size() == 0)
		{
			throw TokenBasedError(p_file, "No symbol [" + nameToken.content + "] found" + DEBUG_INFORMATION, nameToken);
		}

		Type* result = targetSymbolArray->back().returnType;

		if (value->resultAccessor != nullptr)
		{
			for (size_t i = 0; i < value->resultAccessor->tokens.size(); i++)
			{
				std::string expectedAttributeName = value->resultAccessor->tokens[i].content;
				const auto& it = std::find_if(result->attributes.begin(), result->attributes.end(), [expectedAttributeName](const Type::Attribute& p_attribute) {
					return (p_attribute.name == expectedAttributeName);
					});

				if (it == result->attributes.end())
				{
					throw TokenBasedError(p_file, "Attribute [" + expectedAttributeName + "] not found in [" + result->name + "] type" + DEBUG_INFORMATION, value->resultAccessor->tokens[i]);
				}

				result = it->type;
			}
		}

		return (result);
	}

	SemanticChecker::Type* SemanticChecker::getExpressionElementType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionElementInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables)
	{
		switch (p_instruction->type)
		{
		case Instruction::Type::NumberExpressionValue:
		{
			return (getNumberExpressionElementType(p_file, static_pointer_cast<NumberExpressionValueInstruction>(p_instruction)));
		}
		case Instruction::Type::VariableExpressionValue:
		{
			return (getVariableExpressionElementType(p_file, static_pointer_cast<VariableExpressionValueInstruction>(p_instruction), p_variables));
		}
		case Instruction::Type::SymbolCall:
		{
			return (getSymbolCallExpressionElementType(p_file, static_pointer_cast<SymbolCallInstruction>(p_instruction)));
		}
		default:
			return (type("void"));
		}
	}

	SemanticChecker::Type* SemanticChecker::getExpressionType(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables)
	{
		Type* result = nullptr;

		for (const auto& element : p_instruction->elements)
		{
			Type* elementType = getExpressionElementType(p_file, element, p_variables);

			if (elementType == nullptr)
				throw TokenBasedError(p_file, "No type found for token [" + element->mergedToken().content + "]", element->mergedToken());

			if (result != nullptr && result != elementType && result->acceptedConversion.contains(elementType) == false)
			{
				throw TokenBasedError(p_file, "No convertion from type [" + elementType->name + "] to expected type [" + result->name + "]" + DEBUG_INFORMATION, element->mergedToken());
			}
			result = elementType;
		}

		if (result == nullptr)
			return (type("void"));
		return (result);
	}

	void SemanticChecker::checkSymbolCallInstruction(const std::filesystem::path& p_file, const std::shared_ptr<SymbolCallInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables, Type* p_expectedType)
	{
		Token nameToken = Token::merge(p_instruction->name->tokens, Token::Type::Identifier);
		std::vector<Symbol>* targetSymbolArray = symbolArray(nameToken.content);

		if (targetSymbolArray == nullptr || targetSymbolArray->size() == 0)
		{
			throw TokenBasedError(p_file, "No symbol [" + nameToken.content + "] found" + DEBUG_INFORMATION, nameToken);
		}

		Type* symbolReturnType = targetSymbolArray->front().returnType;

		if (p_instruction->resultAccessor != nullptr)
		{
			for (size_t i = 0; i < p_instruction->resultAccessor->tokens.size(); i++)
			{
				std::string expectedAttributeName = p_instruction->resultAccessor->tokens[i].content;
				const auto& it = std::find_if(symbolReturnType->attributes.begin(), symbolReturnType->attributes.end(), [expectedAttributeName](const Type::Attribute& p_attribute) {
					return (p_attribute.name == expectedAttributeName);
					});

				if (it == symbolReturnType->attributes.end())
				{
					throw TokenBasedError(p_file, "Attribute [" + expectedAttributeName + "] not found in [" + symbolReturnType->name + "] type" + DEBUG_INFORMATION, p_instruction->resultAccessor->tokens[i]);
				}

				symbolReturnType = it->type;
			}
		}

		if (p_expectedType != symbolReturnType && p_expectedType->acceptedConversion.contains(symbolReturnType) == false)
		{
			throw TokenBasedError(p_file, "No convertion from type [" + symbolReturnType->name + "] to expected type [" + p_expectedType->name + "]" + DEBUG_INFORMATION, nameToken);
		}

		bool validated = false;
		for (const auto& symbol : *targetSymbolArray)
		{
			if (symbol.parameters.size() == p_instruction->arguments.size())
			{
				bool error = false;
				for (size_t i = 0; i < p_instruction->arguments.size(); i++)
				{
					const Symbol::Parameter& param = symbol.parameters[i];

					Type* parameterType = getExpressionType(p_file, p_instruction->arguments[i], p_variables);

					if (parameterType != param.type && param.type->acceptedConversion.contains(parameterType) == false)
					{
						error = true;
					}
				}

				if (error == false)
				{
					validated = true;
					break;
				}
			}
			if (validated == true)
				break;
		}

		if (validated == false)
		{
			std::string error = "No symbol [" + nameToken.content + "] found with matching types [";
			for (size_t i = 0; i < p_instruction->arguments.size(); i++)
			{
				if (i != 0)
					error += ", ";
				Type* paramType = getExpressionType(p_file, p_instruction->arguments[i], p_variables);
				if (paramType == nullptr)
					error += "Unknow type";
				else
					error += paramType->name;
			}
			error += "]";
			throw TokenBasedError(p_file, error + DEBUG_INFORMATION, nameToken);
		}

		for (const auto& argument : p_instruction->arguments)
		{
			checkExpressionInstruction(p_file, argument, p_variables, getExpressionType(p_file, argument, p_variables));
		}
	}

	void SemanticChecker::checkVariableExpressionValueInstruction(const std::filesystem::path& p_file, const std::shared_ptr<VariableExpressionValueInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables, Type* p_expectedType)
	{
		Token variableToken = Token::merge(p_instruction->tokens, Token::Type::Identifier);
		Type* variableType = getExpressionElementType(p_file, static_pointer_cast<ExpressionElementInstruction>(p_instruction), p_variables);

		if (p_expectedType != variableType && p_expectedType->acceptedConversion.contains(variableType) == false)
		{
			throw TokenBasedError(p_file, "No convertion from type [" + variableType->name + "] to expected type [" + p_expectedType->name + "]" + DEBUG_INFORMATION, variableToken);
		}
	}

	void SemanticChecker::checkNumberExpressionValueInstruction(const std::filesystem::path& p_file, const std::shared_ptr<NumberExpressionValueInstruction>& p_instruction, Type* p_expectedType)
	{
		Type* numberType = getNumberExpressionElementType(p_file, p_instruction);

		if (p_expectedType != numberType && p_expectedType->acceptedConversion.contains(numberType) == false)
		{
			throw TokenBasedError(p_file, "No convertion from type [" + numberType->name + "] to expected type [" + p_expectedType->name + "]" + DEBUG_INFORMATION, p_instruction->mergedToken());
		}
	}

	void SemanticChecker::checkExpressionInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const std::unordered_map<std::string, SemanticChecker::Type*>& p_variables, SemanticChecker::Type* p_expectedType)
	{
		for (const auto& element : p_instruction->elements)
		{
			try
			{
				switch (element->type)
				{
				case Instruction::Type::VariableExpressionValue:
				{
					checkVariableExpressionValueInstruction(p_file, static_pointer_cast<VariableExpressionValueInstruction>(element), p_variables, p_expectedType);
					break;
				}
				case Instruction::Type::NumberExpressionValue:
				{
					checkNumberExpressionValueInstruction(p_file, static_pointer_cast<NumberExpressionValueInstruction>(element), p_expectedType);
					break;
				}
				case Instruction::Type::SymbolCall:
				{
					checkSymbolCallInstruction(p_file, static_pointer_cast<SymbolCallInstruction>(element), p_variables, p_expectedType);
					break;
				}
				default:
				{
					throw TokenBasedError(p_file, "Unexpected expression instruction type : " + ::to_string(element->type) + DEBUG_INFORMATION, Token());
				}
				}
			}
			catch (TokenBasedError& e)
			{
				_result.errors.push_back(e);
			}
		}
	}
}