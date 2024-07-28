#pragma once

#include <string>
#include <vector>
#include "lumina_token.hpp"

namespace Lumina
{
	struct AbstractInstruction
	{
		virtual std::string string() const = 0;
	};

	using Instruction = AbstractInstruction;

	struct IdentifierInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		std::string string() const
		{
			return (token.content);
		}
	};

	struct TypeInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		std::string string() const
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

		std::string string() const
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

		std::string string() const
		{
			return ("Pipeline from [" + inputPipeline.content + "] to [" + outputPipeline.content + "] -> type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct BlockElementInstruction : public AbstractInstruction
	{
		std::shared_ptr<TypeInstruction> type;
		std::shared_ptr<IdentifierInstruction> name;

		std::string string() const
		{
			return ("Type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct StructureBlockInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;
		std::vector<std::shared_ptr<Instruction>> elements;

		std::string string() const
		{
			std::string result = "Structure named [" + name->string() + "] contain : \n";
			for (const auto& element : elements)
			{
				result += "    " + element->string() + "\n";
			}
			return (result);
		}
	};

	struct AttributeBlockInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;
		std::vector<std::shared_ptr<Instruction>> elements;

		std::string string() const
		{
			std::string result = "Attribute named [" + name->string() + "] contain : \n";
			for (const auto& element : elements)
			{
				result += "    " + element->string() + "\n";
			}
			return (result);
		}
	};

	struct ConstantBlockInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;
		std::vector<std::shared_ptr<Instruction>> elements;

		std::string string() const
		{
			std::string result = "Constant named [" + name->string() + "] contain : \n";
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

		std::string string() const
		{
			return ("Texture named [" + name->string() + "]");
		}
	};

	struct NamespaceInstruction : public AbstractInstruction
	{
		std::shared_ptr<Instruction> name;
		std::vector<std::shared_ptr<Instruction>> instructions;

		std::string string() const
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

		std::string string() const
		{
			return ("Type [" + type->string() + "] and name [" + name->string() + "]");
		}
	};

	struct OperatorExpressionInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		std::string string() const
		{
			return (token.content);
		}
	};

	struct NumberExpressionValueInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		std::string string() const
		{
			return (token.content);
		}
	};

	struct StringLiteralsExpressionValueInstruction : public AbstractInstruction
	{
		Lumina::Token token;

		std::string string() const
		{
			return (token.content);
		}
	};

	struct VariableExpressionValueInstruction : public AbstractInstruction
	{
		std::vector<Lumina::Token> tokens;

		std::string string() const
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

		std::string string() const
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

		std::string string() const
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

		std::string string() const
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

		std::string string() const override
		{
			std::string result = "return " + argument->string();
			return result;
		}
	};

	struct DiscardInstruction : public AbstractInstruction
	{
		std::string string() const override
		{
			return ("discarding");
		}
	};

	struct ConditionInstruction : public AbstractInstruction
	{
		std::shared_ptr<ExpressionInstruction> expression;

		std::string string() const override
		{
			return ("Condition : " + expression->string());
		}
	};

	struct ElseInstruction : public AbstractInstruction
	{
		std::shared_ptr<ConditionInstruction> condition;
		std::shared_ptr<SymbolBodyInstruction> body;

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
		std::shared_ptr< SymbolBodyInstruction> body;

		std::string string() const
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

		std::string string() const
		{
			std::string result = "Pipeline flow : " + pipelineToken.content + "\n";

			result += body->string();

			return (result);
		}
	};
}