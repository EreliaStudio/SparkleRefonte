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

	void SemanticChecker::checkSymbolCallReturnType(const std::filesystem::path& p_file, const std::shared_ptr<OperatorExpressionInstruction>& p_instruction, Type* p_expectedType)
	{

	}

	void SemanticChecker::checkExpressionInstruction(const std::filesystem::path& p_file, const std::shared_ptr<ExpressionInstruction>& p_instruction, const std::unordered_map<std::string, Type*>& p_variables, Type* p_expectedType)
	{
		Type* exprType = nullptr;

		// Iterate over the elements in the expression
		for (const auto& element : p_instruction->elements)
		{
			switch (element->type)
			{
			case Instruction::Type::NumberExpressionValue:
				exprType = type("float"); // Assume numeric literals are floats; adjust as necessary
				break;
			case Instruction::Type::VariableExpressionValue:
			{
				auto varExpr = std::static_pointer_cast<VariableExpressionValueInstruction>(element);
				std::string varName = varExpr->tokens.front().content;

				if (!p_variables.contains(varName))
				{
					throw TokenBasedError(p_file, "Variable [" + varName + "] not declared in this scope", varExpr->tokens.front());
				}
				exprType = p_variables.at(varName);
				break;
			}
			case Instruction::Type::SymbolCall:
			{
				auto symbolCall = std::static_pointer_cast<SymbolCallInstruction>(element);
				exprType = checkSymbolCallReturnType(p_file, symbolCall, p_variables);
				break;
			}
			case Instruction::Type::OperatorExpression:
			{
				checkOperatorExpression(p_file, std::static_pointer_cast<OperatorExpressionInstruction>(element));
				break;
			}
			default:
				throw TokenBasedError(p_file, "Unexpected expression type", Token());
			}

			if (p_expectedType != nullptr && exprType != nullptr && exprType != p_expectedType)
			{
				throw TokenBasedError(p_file, "Expression type mismatch. Expected [" + p_expectedType->name + "] but got [" + exprType->name + "]", Token());
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
			checkExpression(p_file, p_instruction->initializer, p_variables, varType);
		}
	}
}