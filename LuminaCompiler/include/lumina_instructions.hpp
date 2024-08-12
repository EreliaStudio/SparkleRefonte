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
			ResultAccessor,
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
		size_t nbElement;
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
		ExpressionElementInstruction(const AbstractInstruction::Type& p_type) :
			AbstractInstruction(p_type)
		{

		}

		virtual Token mergedToken() const = 0;
	};

	struct OperatorExpressionInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		OperatorExpressionInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::OperatorExpression)
		{

		}

		Token mergedToken() const
		{
			return (token);
		}
	};

	struct NumberExpressionValueInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		NumberExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::NumberExpressionValue)
		{

		}

		Token mergedToken() const
		{
			return (token);
		}
	};

	struct StringLiteralsExpressionValueInstruction : public ExpressionElementInstruction
	{
		Lumina::Token token;

		StringLiteralsExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::StringLiteralsExpressionValue)
		{

		}

		Token mergedToken() const
		{
			return (token);
		}
	};

	struct VariableExpressionValueInstruction : public ExpressionElementInstruction
	{
		std::vector<Lumina::Token> tokens;

		VariableExpressionValueInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::VariableExpressionValue)
		{

		}

		Token mergedToken() const
		{
			return (Token::merge(tokens, Token::Type::Identifier));
		}
	};

	struct ExpressionInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<ExpressionElementInstruction>> elements;

		ExpressionInstruction() :
			AbstractInstruction(AbstractInstruction::Type::Expression)
		{

		}

		Token mergedToken() const;
	};

	struct SymbolNameInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		SymbolNameInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolName)
		{

		}
	};


	struct SymbolCallInstruction : public ExpressionElementInstruction
	{
		struct ResultAccessor : public AbstractInstruction
		{
			std::vector<Token> tokens;

			ResultAccessor() :
				AbstractInstruction(AbstractInstruction::Type::ResultAccessor)
			{

			}
		};

		std::shared_ptr<SymbolNameInstruction> name;
		std::vector<std::shared_ptr<ExpressionInstruction>> arguments;
		std::shared_ptr<ResultAccessor> resultAccessor;

		SymbolCallInstruction() :
			ExpressionElementInstruction(AbstractInstruction::Type::SymbolCall)
		{

		}

		Token mergedToken() const
		{
			std::vector<Token> tokens;

			for (const auto& token : name->tokens)
			{
				tokens.push_back(token);
			}

			for (const auto& argument : arguments)
			{
				tokens.push_back(argument->mergedToken());
			}

			return (Token::merge(tokens, Token::Type::Identifier));
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

	struct SymbolBodyInstruction : public AbstractInstruction
	{
		std::vector<std::shared_ptr<Instruction>> elements;

		SymbolBodyInstruction() :
			AbstractInstruction(AbstractInstruction::Type::SymbolBody)
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