#pragma once

#include <string>
#include <vector>
#include "lumina_token.hpp"

namespace Lumina
{
	struct AbstractInstruction
	{
		enum class Type
		{
			Identifier,
			Type,
			Include,
			PipelineFlow,
			BlockElement,
			StructureBlock,
			AttributeBlock,
			ConstantBlock,
			Texture,
			Namespace,
			SymbolParameter,
			OperatorExpression,
			NumberExpressionValue,
			StringLiteralsExpressionValue,
			VariableExpressionValue,
			Expression,
			VariableDesignation,
			VariableAssignation,
			VariableDeclaration,
			SymbolName,
			SymbolBody,
			SymbolCall,
			Return,
			Discard,
			Condition,
			Else,
			IfStatement,
			WhileLoop,
			ForLoop,
			Symbol,
			PipelineBody
		};

		Type type;

		AbstractInstruction(const Type& p_type) :
			type(p_type)
		{

		}
		virtual std::string string() const = 0;
	};

	using Instruction = AbstractInstruction;

	struct IdentifierInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		IdentifierInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Identifier)
		{

		}

		std::string string() const override
		{
			return (token.content);
		}
	};

	struct TypeInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		TypeInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Type)
		{

		}

		std::string string() const override
		{
			std::string result = "";

			for (const auto& token : tokens)
			{
				result += token.content;
			}

			return (result);
		}
	};

	struct IncludeInstruction : public AbstractInstruction
	{
		Lumina::Token includeFile;

		IncludeInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Include)
		{

		}

		std::string string() const override
		{
			return ("Include file : " + includeFile.content.substr(1, includeFile.content.size() - 2));
		}
	};

	struct PipelineFlowInstruction : public AbstractInstruction
	{
		Lumina::Token inputPipeline;
		Lumina::Token outputPipeline;
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;

		PipelineFlowInstruction() :
			AbstractInstruction(AbstractInstruction::Type::PipelineFlow)
		{

		}

		std::string string() const override
		{
			return ("Pipeline from [" + inputPipeline.content + "] to [" + outputPipeline.content + "] -> type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct BlockElementInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;

		BlockElementInstruction() :
			AbstractInstruction(AbstractInstruction::Type::BlockElement)
		{

		}

		std::string string() const override
		{
			return ("Type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct StructureBlockInstruction : public AbstractInstruction
	{
		Lumina::Token name;
		std::vector<std::shared_ptr<BlockElementInstruction>> elements;

		StructureBlockInstruction() :
			AbstractInstruction(AbstractInstruction::Type::StructureBlock)
		{

		}

		std::string string() const override
		{
			std::string result = "Structure named [" + name.content + "] contain : \n";
			for (const auto& element : elements)
			{
				result += "    " + element->string() + "\n";
			}
			return (result);
		}
	};

	struct AttributeBlockInstruction : public AbstractInstruction
	{
		Lumina::Token name;
		std::vector<std::shared_ptr<BlockElementInstruction>> elements;

		AttributeBlockInstruction() :
			AbstractInstruction(AbstractInstruction::Type::AttributeBlock)
		{

		}

		std::string string() const override
		{
			std::string result = "Attribute named [" + name.content + "] contain : \n";
			for (const auto& element : elements)
			{
				result += "    " + element->string() + "\n";
			}
			return (result);
		}
	};

	struct ConstantBlockInstruction : public AbstractInstruction
	{
		Lumina::Token name;
		std::vector<std::shared_ptr<BlockElementInstruction>> elements;

		ConstantBlockInstruction() :
			AbstractInstruction(AbstractInstruction::Type::ConstantBlock)
		{

		}

		std::string string() const override
		{
			std::string result = "Constant named [" + name.content + "] contain : \n";
			for (const auto& element : elements)
			{
				result += "    " + element->string() + "\n";
			}
			return (result);
		}
	};

	struct TextureInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;

		TextureInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Texture)
		{

		}

		std::string string() const override
		{
			return ("Texture named [" + name->string() + "]");
		}
	};

	struct NamespaceInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;
		std::vector<std::shared_ptr<Instruction>> instructions;

		NamespaceInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Namespace)
		{

		}

		std::string string() const override
		{
			std::string result = "Namespace contain [" + std::to_string(instructions.size()) + "] instructions :\n";
			for (const auto& instruction : instructions)
			{
				result += "    " + instruction->string() + "\n";
			}
			return (result);
		}
	};

	struct SymbolParameterInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;

		SymbolParameterInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolParameter)
		{

		}

		std::string string() const override
		{
			return ("Type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct OperatorExpressionInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		OperatorExpressionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::OperatorExpression)
		{

		}

		std::string string() const override
		{
			return (token.content);
		}
	};

	struct NumberExpressionValueInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		NumberExpressionValueInstruction() :
			AbstractInstruction(AbstractInstruction::Type::NumberExpressionValue)
		{

		}

		std::string string() const override
		{
			return (token.content);
		}
	};

	struct StringLiteralsExpressionValueInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		StringLiteralsExpressionValueInstruction() :
			AbstractInstruction(AbstractInstruction::Type::StringLiteralsExpressionValue)
		{

		}

		std::string string() const override
		{
			return (token.content);
		}
	};

	struct VariableExpressionValueInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		VariableExpressionValueInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableExpressionValue)
		{

		}

		std::string string() const override
		{
			std::string result = "";

			for (const auto& token : tokens)
			{
				result += token.content;
			}

			return (result);
		}
	};

	struct ExpressionInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<Instruction>> elements;

		ExpressionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Expression)
		{

		}

		std::string string() const override
		{
			std::string result;

			for (size_t i = 0; i < elements.size(); i++)
			{
				if (i != 0)
					result += " ";
				result += elements[i]->string();
			}

			return (result);
		}
	};

	struct VariableDesignationInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		VariableDesignationInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableDesignation)
		{

		}

		std::string string() const override
		{
			std::string result = "";

			for (const auto& token : tokens)
			{
				result += token.content;
			}

			return (result);
		}
	};

	struct VariableAssignationInstruction : public AbstractInstruction
	{
		std::shared_ptr<VariableDesignationInstruction> name;
		std::shared_ptr<ExpressionInstruction> initializer;

		VariableAssignationInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableAssignation)
		{

		}

		std::string string() const override
		{
			std::string result = "Variable Assignation: " + name->string() + " = " + initializer->string();

			return result;
		}
	};

	struct VariableDeclarationInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;
		std::shared_ptr<ExpressionInstruction> initializer;

		VariableDeclarationInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableDeclaration)
		{

		}

		std::string string() const override
		{
			std::string result = "Variable Declaration: " + type->string() + " " + name->string();
			if (initializer)
			{
				result += " = " + initializer->string();
			}
			return result;
		}
	};

	struct SymbolNameInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		SymbolNameInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolName)
		{

		}

		std::string string() const override
		{
			std::string result = "";

			for (const auto& token : tokens)
			{
				result += token.content;
			}

			return (result);
		}
	};

	struct SymbolBodyInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<Instruction>> elements;

		SymbolBodyInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolBody)
		{

		}

		std::string string() const override
		{
			std::string result = "";

			for (const auto& element : elements)
			{
				result += element->string() + "\n";
			}

			return (result);
		}
	};

	struct SymbolCallInstruction : public AbstractInstruction
	{
		std::shared_ptr<SymbolNameInstruction> name;
		std::vector<std::shared_ptr<ExpressionInstruction>> arguments;

		SymbolCallInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolCall)
		{

		}

		std::string string() const override
		{
			std::string result = "Symbol Call: " + name->string() + "(";
			for (size_t i = 0; i < arguments.size(); ++i)
			{
				if (i != 0)
				{
					result += ", ";
				}
				result += arguments[i]->string();
			}
			result += ")";
			return result;
		}
	};

	struct ReturnInstruction : public AbstractInstruction
	{
		std::shared_ptr<ExpressionInstruction> argument;

		ReturnInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Return)
		{

		}

		std::string string() const override
		{
			std::string result = "return " + argument->string();
			return result;
		}
	};

	struct DiscardInstruction : public AbstractInstruction
	{
		DiscardInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Discard)
		{

		}

		std::string string() const override
		{
			return ("discarding");
		}
	};

	struct ConditionInstruction : public AbstractInstruction
	{
		std::shared_ptr<ExpressionInstruction> expression;

		ConditionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Condition)
		{

		}

		std::string string() const override
		{
			return ("Condition : " + expression->string());
		}
	};

	struct ElseInstruction : public AbstractInstruction
	{
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<SymbolBodyInstruction> body;

		ElseInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Else)
		{

		}

		std::string string() const override
		{
			if (condition)
			{
				return "Else If (" + condition->string() + ") " + body->string();
			}
			else
			{
				return "Else " + body->string();
			}
		}
	};

	struct IfStatementInstruction : public AbstractInstruction
	{
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<SymbolBodyInstruction> body;
		std::vector<std::shared_ptr<ElseInstruction>> elseBlocks;

		IfStatementInstruction() :
			AbstractInstruction(AbstractInstruction::Type::IfStatement)
		{

		}

		std::string string() const override
		{
			std::string result = "If (" + condition->string() + ") " + body->string();
			for (const auto& elseBlock : elseBlocks)
			{
				result += "\n" + elseBlock->string();
			}
			return result;
		}
	};

	struct WhileLoopInstruction : public AbstractInstruction
	{
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<SymbolBodyInstruction> body;

		WhileLoopInstruction() :
			AbstractInstruction(AbstractInstruction::Type::WhileLoop)
		{

		}

		std::string string() const override
		{
			return "While (" + condition->string() + ") " + body->string();
		}
	};

	struct ForLoopInstruction : public AbstractInstruction
	{
		std::shared_ptr<AbstractInstruction> initializer;
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<ExpressionInstruction> increment;
		std::shared_ptr<SymbolBodyInstruction> body;

		ForLoopInstruction() :
			AbstractInstruction(AbstractInstruction::Type::ForLoop)
		{

		}

		std::string string() const override
		{
			return "For (" + initializer->string() + "; " + condition->string() + "; " + increment->string() + ") " + body->string();
		}
	};

	struct SymbolInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> returnType;
		std::shared_ptr<IdentifierInstruction> name;
		std::vector<std::shared_ptr<SymbolParameterInstruction>> parameters;
		std::shared_ptr<SymbolBodyInstruction> body;

		SymbolInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Symbol)
		{

		}

		std::string string() const override
		{
			std::string result = "Symbol [" + name->string() + "] return [" + returnType->string() + "] and take [";
			for (size_t i = 0; i < parameters.size(); i++)
			{
				if (i != 0)
					result += " / ";
				result += parameters[i]->string();
			}
			result += "] as parameters and contain body :\n";
			result += body->string();

			return (result);
		}
	};

	struct PipelineBodyInstruction : public AbstractInstruction
	{
		Lumina::Token pipelineToken;
		std::shared_ptr<SymbolBodyInstruction> body;

		PipelineBodyInstruction() :
			AbstractInstruction(AbstractInstruction::Type::PipelineBody)
		{

		}

		std::string string() const override
		{
			std::string result = "Pipeline flow : " + pipelineToken.content + "\n";

			result += body->string();

			return (result);
		}
	};
}
