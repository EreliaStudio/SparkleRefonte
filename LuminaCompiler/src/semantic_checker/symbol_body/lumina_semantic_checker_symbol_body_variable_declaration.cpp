#include "lumina_semantic_checker.hpp"

namespace Lumina
{
	void SemanticChecker::checkOperatorExpression(const std::filesystem::path& p_file, const std::shared_ptr<OperatorExpressionInstruction>& p_instruction, Type* p_expectedType)
	{
		if (std::find(p_expectedType->operators.begin(), p_expectedType->operators.end(), p_instruction->token.content) == p_expectedType->operators.end())
		{
			throw TokenBasedError(p_file, "Operator [" + p_instruction->token.content + "] not supported by type [" + p_expectedType->name + "]", p_instruction->token);
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

	void SemanticChecker::checkVariableDeclarationInstruction(const std::filesystem::path& p_file, const std::shared_ptr<VariableDeclarationInstruction>& p_instruction, std::unordered_map<std::string, SemanticChecker::Type*>& p_variables)
	{
		Token typeToken = Token::merge(p_instruction->type->tokens, Token::Type::Identifier);
		std::string typeName = typeToken.content;
		Type* varType = type(typeName);
		if (varType == nullptr)
		{
			throw TokenBasedError(p_file, "Type [" + typeName + "] not found", typeToken);
		}

		if (p_variables.contains(p_instruction->name.content))
		{
			throw TokenBasedError(p_file, "Variable [" + p_instruction->name.content + "] already declared in this scope", p_instruction->name);
		}

		p_variables[p_instruction->name.content] = varType;

		if (p_instruction->initializer)
		{
			checkExpressionInstruction(p_file, p_instruction->initializer, p_variables, varType);
		}
	}
}