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
		virtual ~AbstractInstruction()
		{

		}
	};

	using Instruction = AbstractInstruction;

	struct IdentifierInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		IdentifierInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Identifier)
		{

		}
	};

	struct TypeInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		TypeInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Type)
		{

		}
	};

	struct IncludeInstruction : public AbstractInstruction
	{
		Lumina::Token includeFile;

		IncludeInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Include)
		{

		}
	};

	struct PipelineFlowInstruction : public AbstractInstruction
	{
		Lumina::Token inputPipeline;
		Lumina::Token outputPipeline;
		std::shared_ptr<TypeInstruction> type;
		Lumina::Token name;

		PipelineFlowInstruction() :
			AbstractInstruction(AbstractInstruction::Type::PipelineFlow)
		{

		}
	};

	struct BlockElementInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		Lumina::Token name;

		BlockElementInstruction()
		{

		}
	};

	struct BlockInstruction : public AbstractInstruction
	{
		Lumina::Token name;
		std::vector<std::shared_ptr<BlockElementInstruction>> elements;

		BlockInstruction(const Type& p_type) :
			AbstractInstruction(p_type)
		{

		}
	};

	struct StructureBlockInstruction : public BlockInstruction
	{
		StructureBlockInstruction() :
			BlockInstruction(AbstractInstruction::Type::StructureBlock)
		{

		}
	};

	struct AttributeBlockInstruction : public BlockInstruction
	{
		AttributeBlockInstruction() :
			BlockInstruction(AbstractInstruction::Type::AttributeBlock)
		{

		}
	};

	struct ConstantBlockInstruction : public BlockInstruction
	{
		ConstantBlockInstruction() :
			BlockInstruction(AbstractInstruction::Type::ConstantBlock)
		{

		}
	};

	struct TextureInstruction : public AbstractInstruction
	{
		Lumina::Token name;

		TextureInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Texture)
		{

		}
	};

	struct NamespaceInstruction : public AbstractInstruction
	{
		Lumina::Token name;
		std::vector<std::shared_ptr<Instruction>> instructions;

		NamespaceInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Namespace)
		{

		}
	};

	struct SymbolParameterInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		Lumina::Token name;

		SymbolParameterInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolParameter)
		{

		}
	};

	struct ExpressionElementInstruction : public AbstractInstruction
	{
		virtual std::vector<Token> getTokens() const = 0;

		ExpressionElementInstruction(const AbstractInstruction::Type& p_type) :
			AbstractInstruction(p_type)
		{

		}
	};

	struct OperatorExpressionInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		std::vector<Token> getTokens() const
		{
			std::vector<Token> result;

			result.push_back(token);

			return (result);
		}

		OperatorExpressionInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::OperatorExpression)
		{

		}
	};

	struct NumberExpressionValueInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		std::vector<Token> getTokens() const
		{
			std::vector<Token> result;

			result.push_back(token);

			return (result);
		}

		NumberExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::NumberExpressionValue)
		{

		}
	};

	struct StringLiteralsExpressionValueInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		std::vector<Token> getTokens() const
		{
			std::vector<Token> result;

			result.push_back(token);

			return (result);
		}

		StringLiteralsExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::StringLiteralsExpressionValue)
		{

		}
	};

	struct VariableExpressionValueInstruction : public ExpressionElementInstruction
	{
		std::vector<Lumina::Token> tokens;

		std::vector<Token> getTokens() const
		{
			return (tokens);
		}

		VariableExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::VariableExpressionValue)
		{

		}
	};

	struct ExpressionInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<ExpressionElementInstruction>> elements;

		ExpressionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Expression)
		{

		}

		std::vector<Lumina::Token> getTokens() const
		{
			std::vector<Lumina::Token> result;

			for (const auto& element : elements)
			{
				std::vector<Lumina::Token> elementTokens = element->getTokens();

				result.insert(result.end(), elementTokens.begin(), elementTokens.end());
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
	};

	struct VariableAssignationInstruction : public AbstractInstruction
	{
		std::shared_ptr<VariableDesignationInstruction> name;
		std::shared_ptr<ExpressionInstruction> initializer;

		VariableAssignationInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableAssignation)
		{

		}
	};

	struct VariableDeclarationInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		Lumina::Token name;
		std::shared_ptr<ExpressionInstruction> initializer;

		VariableDeclarationInstruction() :
			AbstractInstruction(AbstractInstruction::Type::VariableDeclaration)
		{

		}
	};

	struct SymbolNameInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		SymbolNameInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolName)
		{

		}
	};

	struct SymbolBodyInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<Instruction>> elements;

		SymbolBodyInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolBody)
		{

		}
	};

	struct SymbolCallInstruction : public ExpressionElementInstruction
	{
		std::shared_ptr<SymbolNameInstruction> name;
		std::vector<std::shared_ptr<ExpressionInstruction>> arguments;

		std::vector<Lumina::Token> getTokens() const
		{
			std::vector<Lumina::Token> result;

			for (const auto& token : name->tokens)
			{
				result.push_back(token);
			}

			for (const auto& argument : arguments)
			{
				std::vector<Lumina::Token> tmp = argument->getTokens();
				for (const auto& token : tmp)
				{
					result.push_back(token);
				}
			}

			return (result);
		}

		SymbolCallInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::SymbolCall)
		{

		}
	};

	struct ReturnInstruction : public AbstractInstruction
	{
		std::shared_ptr<ExpressionInstruction> argument;

		ReturnInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Return)
		{

		}
	};

	struct DiscardInstruction : public AbstractInstruction
	{
		DiscardInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Discard)
		{

		}
	};

	struct ConditionInstruction : public AbstractInstruction
	{
		std::shared_ptr<ExpressionInstruction> expression;

		ConditionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Condition)
		{

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
	};

	struct WhileLoopInstruction : public AbstractInstruction
	{
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<SymbolBodyInstruction> body;

		WhileLoopInstruction() :
			AbstractInstruction(AbstractInstruction::Type::WhileLoop)
		{

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
	};

	struct SymbolInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> returnType;
		Lumina::Token name;
		std::vector<std::shared_ptr<SymbolParameterInstruction>> parameters;
		std::shared_ptr<SymbolBodyInstruction> body;

		SymbolInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Symbol)
		{

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
	};
}

std::string to_string(Lumina::AbstractInstruction::Type p_type);