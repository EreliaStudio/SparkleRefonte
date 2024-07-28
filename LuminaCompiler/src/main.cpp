#include <fstream>

#include "lumina_utils.hpp"
#include "lumina_token.hpp"
#include "lumina_tokenizer.hpp"

#include "lumina_exception.hpp"

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

class Lexer
{
public:
	struct Result
	{
		std::vector<std::shared_ptr<Instruction>> instructions;
		std::vector<Lumina::TokenBasedError> errors;
	};
	
private:
	std::filesystem::path _file;
	Result _result;
	Lumina::Token noToken;
	const std::vector<Lumina::Token>* _tokens;
	size_t _index = 0;

	bool hasTokenLeft() const
	{
		return (_index < _tokens->size());
	}

	void backOff()
	{
		_index--;
	}
	
	void advance()
	{
		_index++;
	}

	const Lumina::Token& currentToken() const
	{
		return (_tokens->operator[](_index));
	}

	const Lumina::Token& tokenAtIndex(size_t p_index) const
	{
		if (_index + p_index >= _tokens->size())
			return noToken;
		return (_tokens->operator[](_index + p_index));
	}

	const Lumina::Token& nextToken() const
	{
		return (tokenAtIndex(1));
	}

	void skipToken()
	{
		_index++;
	}

	void skipLine()
	{
		int currentLine = currentToken().context.line;
		while (hasTokenLeft() == true &&
			currentLine == currentToken().context.line)
		{
			skipToken();
		}
	}

	const Lumina::Token& expect(Lumina::Token::Type p_expectedType, const std::string& p_errorMessage = "Unexpected token type.")
	{
		if (currentToken().type != p_expectedType)
		{
			throw Lumina::TokenBasedError(_file, p_errorMessage, currentToken());
		}
		const Lumina::Token& result = currentToken();
		advance();
		return (result);
	}

	const Lumina::Token& expect(std::vector<Lumina::Token::Type> p_expectedTypes, const std::string& p_errorMessage = "Unexpected token type.")
	{
		bool found = false;

		for (const auto& type : p_expectedTypes)
		{
			if (currentToken().type == type)
			{
				found = true;
			}
		}

		if (found == false)
		{
			throw Lumina::TokenBasedError(_file, p_errorMessage, currentToken());
		}
		const Lumina::Token& result = currentToken();
		advance();
		return (result);
	}

	std::shared_ptr<IncludeInstruction> parseIncludeInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<IncludeInstruction> result = std::make_shared<IncludeInstruction>();

		expect(Lumina::Token::Type::Include, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->includeFile = expect({ Lumina::Token::Type::StringLitteral, Lumina::Token::Type::IncludeLitteral }, "Expected a valid include file token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return (result);
	}

	std::shared_ptr<TypeInstruction> parseTypeInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<TypeInstruction> result = std::make_shared<TypeInstruction>();

		if (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Unexpected token found." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<IdentifierInstruction> parseIdentifierInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<IdentifierInstruction> result = std::make_shared<IdentifierInstruction>();

		result->token = expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<PipelineFlowInstruction> parsePipelineFlowInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<PipelineFlowInstruction> result = std::make_shared<PipelineFlowInstruction>();

		result->inputPipeline = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::PipelineFlowSeparator, "Expected a pipeline flow separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->outputPipeline = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::Separator, "Expected a separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<BlockElementInstruction> parseBlockElementInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<BlockElementInstruction> result = std::make_shared<BlockElementInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<StructureBlockInstruction> parseStructureBlockInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<StructureBlockInstruction> result = std::make_shared<StructureBlockInstruction>();

		expect(Lumina::Token::Type::StructureBlock, "Expected a structure block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<AttributeBlockInstruction> parseAttributeBlockInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<AttributeBlockInstruction> result = std::make_shared<AttributeBlockInstruction>();

		expect(Lumina::Token::Type::AttributeBlock, "Expected an attribute block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ConstantBlockInstruction> parseConstantBlockInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<ConstantBlockInstruction> result = std::make_shared<ConstantBlockInstruction>();

		expect(Lumina::Token::Type::ConstantBlock, "Expected a constant block token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				result->elements.push_back(parseBlockElementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);

				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<TextureInstruction> parseTextureInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<TextureInstruction> result = std::make_shared<TextureInstruction>();

		expect(Lumina::Token::Type::Texture, "Expected a texture token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolParameterInstruction> parseSymbolParameterInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<SymbolParameterInstruction> result = std::make_shared<SymbolParameterInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<OperatorExpressionInstruction> parseOperatorExpressionInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<OperatorExpressionInstruction> result = std::make_shared<OperatorExpressionInstruction>();

		result->token = expect(Lumina::Token::Type::Operator, "Expected an operator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<NumberExpressionValueInstruction> parseNumberExpressionValueInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<NumberExpressionValueInstruction> result = std::make_shared<NumberExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::Number, "Expected a number token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<StringLiteralsExpressionValueInstruction> parseStringLiteralsExpressionValueInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<StringLiteralsExpressionValueInstruction> result = std::make_shared<StringLiteralsExpressionValueInstruction>();

		result->token = expect(Lumina::Token::Type::StringLitteral, "Expected a string literal token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<VariableExpressionValueInstruction> parseVariableExpressionValueInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<VariableExpressionValueInstruction> result = std::make_shared<VariableExpressionValueInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected a namespace separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		while (currentToken().type == Lumina::Token::Type::Accessor)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::Accessor, "Expected an accessor token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<SymbolNameInstruction> parseSymbolCallNameInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<SymbolNameInstruction> result = std::make_shared<SymbolNameInstruction>();

		if (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator, "Expected a namespace separator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<SymbolCallInstruction> parseSymbolCallInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<SymbolCallInstruction> result = std::make_shared<SymbolCallInstruction>();

		result->name = parseSymbolCallNameInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			if (result->arguments.size() != 0)
			{
				expect(Lumina::Token::Type::Comma, "Expected a comma." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
			result->arguments.push_back(parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	bool describeSymbolCallName()
	{
		size_t offset = 0;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			offset++;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::Identifier)
		{
			offset++;

			while (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			{
				offset++;
				if (tokenAtIndex(offset).type != Lumina::Token::Type::Identifier)
				{
					return false;
				}
				offset++;
			}

			if (tokenAtIndex(offset).type == Lumina::Token::Type::OpenParenthesis)
			{
				return true;
			}
		}

		return false;
	}

	std::shared_ptr<ExpressionInstruction> parseExpression(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<ExpressionInstruction> result = std::make_shared<ExpressionInstruction>();

		bool isParsing = true;
		while (isParsing)
		{
			switch (currentToken().type)
			{
			case Lumina::Token::Type::Number:
				result->elements.push_back(parseNumberExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				break;
			case Lumina::Token::Type::StringLitteral:
				result->elements.push_back(parseStringLiteralsExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				break;
			case Lumina::Token::Type::Identifier:
			case Lumina::Token::Type::NamespaceSeparator:
			{
				if (describeSymbolCallName() == true)
				{
					result->elements.push_back(parseSymbolCallInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				}
				else
				{
					result->elements.push_back(parseVariableExpressionValueInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				}
				break;
			}
			}

			if (currentToken().type == Lumina::Token::Type::Operator)
			{
				result->elements.push_back(parseOperatorExpressionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
				isParsing = true;
			}
			else
			{
				isParsing = false;
			}
		}

		return result;
	}

	std::shared_ptr<VariableDeclarationInstruction> parseVariableDeclarationInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<VariableDeclarationInstruction> result = std::make_shared<VariableDeclarationInstruction>();

		result->type = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			expect(Lumina::Token::Type::Assignator, "Expected an assignator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ReturnInstruction> parseReturnInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<ReturnInstruction> result = std::make_shared<ReturnInstruction>();

		expect(Lumina::Token::Type::Return, "Expected a return token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->argument = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<DiscardInstruction> parseDiscardInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<DiscardInstruction> result = std::make_shared<DiscardInstruction>();

		expect(Lumina::Token::Type::Discard, "Expected a discard token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<VariableDesignationInstruction> parseVariableDesignationInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<VariableDesignationInstruction> result = std::make_shared<VariableDesignationInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		while (currentToken().type == Lumina::Token::Type::Accessor)
		{
			expect(Lumina::Token::Type::Accessor, "Expected an accessor token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier, "Expected an identifier token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
		}

		return result;
	}

	std::shared_ptr<VariableAssignationInstruction> parseVariableAssignationInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<VariableAssignationInstruction> result = std::make_shared<VariableAssignationInstruction>();

		result->name = parseVariableDesignationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::Assignator, "Expected an assignator token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}


	bool describeVariableDeclarationInstruction()
	{
		size_t offset = 0;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			offset++;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::Identifier)
		{
			offset++;

			while (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			{
				offset++;
				if (tokenAtIndex(offset).type != Lumina::Token::Type::Identifier)
				{
					return false;
				}
				offset++;
			}

			if (tokenAtIndex(offset).type == Lumina::Token::Type::Identifier)
			{
				offset++;

				if (tokenAtIndex(offset).type == Lumina::Token::Type::Assignator)
				{
					return true;
				}

				return true;
			}
		}

		return false;
	}

	bool describeSymbolCallInstruction()
	{
		size_t offset = 0;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			offset++;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::Identifier)
		{
			offset++;

			while (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator)
			{
				offset++;
				if (tokenAtIndex(offset).type != Lumina::Token::Type::Identifier)
				{
					return false;
				}
				offset++;
			}

			if (tokenAtIndex(offset).type == Lumina::Token::Type::OpenParenthesis)
			{
				return true;
			}
		}

		return false;
	}


	bool describeVariableAssignationInstruction()
	{
		size_t offset = 0;

		if (tokenAtIndex(offset).type == Lumina::Token::Type::Identifier)
		{
			offset++;

			while (tokenAtIndex(offset).type == Lumina::Token::Type::NamespaceSeparator ||
				tokenAtIndex(offset).type == Lumina::Token::Type::Accessor)
			{
				offset++;

				if (tokenAtIndex(offset).type != Lumina::Token::Type::Identifier)
				{
					return false;
				}

				offset++;
			}

			if (tokenAtIndex(offset).type == Lumina::Token::Type::Assignator)
			{
				return true;
			}
		}

		return false;
	}

	std::shared_ptr<ConditionInstruction> parseConditionInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<ConditionInstruction> result = std::make_shared<ConditionInstruction>();

		result->expression = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<IfStatementInstruction> parseIfStatementInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<IfStatementInstruction> result = std::make_shared<IfStatementInstruction>();

		expect(Lumina::Token::Type::IfStatement, "Expected an 'if' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		while (currentToken().type == Lumina::Token::Type::ElseStatement)
		{
			expect(Lumina::Token::Type::ElseStatement, "Expected an 'else' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

			std::shared_ptr<ElseInstruction> elseInstr = std::make_shared<ElseInstruction>();

			if (currentToken().type == Lumina::Token::Type::IfStatement)
			{
				expect(Lumina::Token::Type::IfStatement, "Expected an 'if' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				elseInstr->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
				expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}

			elseInstr->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			result->elseBlocks.push_back(elseInstr);
		}

		return result;
	}

	std::shared_ptr<WhileLoopInstruction> parseWhileLoopInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<WhileLoopInstruction> result = std::make_shared<WhileLoopInstruction>();

		expect(Lumina::Token::Type::WhileStatement, "Expected a 'while' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<ForLoopInstruction> parseForLoopInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<ForLoopInstruction> result = std::make_shared<ForLoopInstruction>();

		expect(Lumina::Token::Type::ForStatement, "Expected a 'for' statement token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			if (describeVariableDeclarationInstruction())
			{
				result->initializer = parseVariableDeclarationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
			else
			{
				result->initializer = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
			}
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::EndOfSentence)
		{
			result->condition = parseConditionInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		if (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			result->increment = parseExpression(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolBodyInstruction> parseSymbolBodyInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<SymbolBodyInstruction> result = std::make_shared<SymbolBodyInstruction>();

		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				switch (currentToken().type)
				{
				case Lumina::Token::Type::Comment:
					skipToken();
					break;
				case Lumina::Token::Type::Identifier:
				case Lumina::Token::Type::NamespaceSeparator:
					if (describeVariableDeclarationInstruction() == true)
					{
						result->elements.push_back(parseVariableDeclarationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					}
					else if (describeSymbolCallInstruction() == true)
					{
						result->elements.push_back(parseSymbolCallInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
						expect(Lumina::Token::Type::EndOfSentence, "Expected end of sentence." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
					}
					else if (describeVariableAssignationInstruction() == true)
					{
						result->elements.push_back(parseVariableAssignationInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					}
					else
					{
						throw Lumina::TokenBasedError(_file, "Unexpected token type: " + to_string(currentToken().type) + COMPOSED_DEBUG_INFORMATION(p_debugInformation), currentToken());
					}
					break;
				case Lumina::Token::Type::Return:
					result->elements.push_back(parseReturnInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::Discard:
					result->elements.push_back(parseDiscardInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::IfStatement:
					result->elements.push_back(parseIfStatementInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::WhileStatement:
					result->elements.push_back(parseWhileLoopInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::ForStatement:
					result->elements.push_back(parseForLoopInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				default:
					throw Lumina::TokenBasedError(_file, "Unexpected token type: " + to_string(currentToken().type) + COMPOSED_DEBUG_INFORMATION(p_debugInformation), currentToken());
				}
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);
				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<SymbolInstruction> parseSymbolInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<SymbolInstruction> result = std::make_shared<SymbolInstruction>();

		result->returnType = parseTypeInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			result->parameters.push_back(parseSymbolParameterInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
			if (currentToken().type != Lumina::Token::Type::CloseParenthesis)
				expect(Lumina::Token::Type::Comma, "Expected a comma." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		}
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<NamespaceInstruction> parseNamespaceInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<NamespaceInstruction> result = std::make_shared<NamespaceInstruction>();

		expect(Lumina::Token::Type::Namespace, "Expected a namespace token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		result->name = parseIdentifierInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenCurlyBracket, "Expected an open curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				switch (currentToken().type)
				{
				case Lumina::Token::Type::Comment:
					skipToken();
					break;
				case Lumina::Token::Type::StructureBlock:
					result->instructions.push_back(parseStructureBlockInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::AttributeBlock:
					result->instructions.push_back(parseAttributeBlockInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::ConstantBlock:
					result->instructions.push_back(parseConstantBlockInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::Texture:
					result->instructions.push_back(parseTextureInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::Namespace:
					result->instructions.push_back(parseNamespaceInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				case Lumina::Token::Type::Identifier:
					result->instructions.push_back(parseSymbolInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation)));
					break;
				default:
					throw Lumina::TokenBasedError(_file, "Unexpected token type : " + to_string(currentToken().type) + COMPOSED_DEBUG_INFORMATION(p_debugInformation), currentToken());
				}
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);
				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket, "Expected a close curly bracket." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}

	std::shared_ptr<PipelineBodyInstruction> parsePipelineBodyInstruction(const std::string& p_debugInformation = "")
	{
		std::shared_ptr<PipelineBodyInstruction> result = std::make_shared<PipelineBodyInstruction>();

		result->pipelineToken = expect(Lumina::Token::Type::PipelineFlow, "Expected a pipeline flow token." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::OpenParenthesis, "Expected an open parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));
		expect(Lumina::Token::Type::CloseParenthesis, "Expected a close parenthesis." + COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		result->body = parseSymbolBodyInstruction(COMPOSED_DEBUG_INFORMATION(p_debugInformation));

		return result;
	}


	Result parse(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens)
	{
		_result = Result();
		_file = p_file;
		_tokens = &p_tokens;
		_index = 0;

		while (hasTokenLeft() == true)
		{
			try
			{
				switch (currentToken().type)
				{
				case Lumina::Token::Type::Comment:
				{
					skipToken();
					break;
				}
				case Lumina::Token::Type::Include:
				{
					_result.instructions.push_back(parseIncludeInstruction());
					break;
				}
				case Lumina::Token::Type::PipelineFlow:
				{
					if (nextToken().type == Lumina::Token::Type::OpenParenthesis)
					{
						_result.instructions.push_back(parsePipelineBodyInstruction());
					}
					else
					{
						_result.instructions.push_back(parsePipelineFlowInstruction());
					}
					break;
				}
				case Lumina::Token::Type::StructureBlock:
				{
					_result.instructions.push_back(parseStructureBlockInstruction());
					break;
				}
				case Lumina::Token::Type::AttributeBlock:
				{
					_result.instructions.push_back(parseAttributeBlockInstruction());
					break;
				}
				case Lumina::Token::Type::ConstantBlock:
				{
					_result.instructions.push_back(parseConstantBlockInstruction());
					break;
				}
				case Lumina::Token::Type::Texture:
				{
					_result.instructions.push_back(parseTextureInstruction());
					break;
				}
				case Lumina::Token::Type::Namespace:
				{
					_result.instructions.push_back(parseNamespaceInstruction());
					break;
				}
				case Lumina::Token::Type::Identifier:
				{
					_result.instructions.push_back(parseSymbolInstruction());
					break;
				}
				default:
				{
					throw Lumina::TokenBasedError(_file, "Unexpected token type : " + to_string(currentToken().type), currentToken());
					break;
				}
				}
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);

				skipLine();
			}
		}

		return (_result);
	}

public:
	Lexer() = default;

	static Result checkSyntax(const std::filesystem::path& p_file, const std::vector<Lumina::Token>& p_tokens)
	{
		return (Lexer().parse(p_file, p_tokens));
	}
};

int main(int argc, char** argv)
{
	if (argc == 1)
	{
		std::cout << "Usage : " << argv[0] << " [path to your lumina shader code]" << std::endl;
		return (0);
	}

	std::string rawInput = Lumina::readFileAsString(argv[1]);

	std::vector<Lumina::Token> tokens = Lumina::Tokenizer::tokenize(rawInput);

	std::fstream ouputStream;

	ouputStream.open("resultToken.txt", std::ios_base::out);

	for (const auto& token : tokens)
	{
		ouputStream << token << std::endl;
	}

	ouputStream.close();

	Lexer::Result lexerResult = Lexer::checkSyntax(argv[1], tokens);

	for (const auto& error : lexerResult.errors)
	{
		std::cout << error.what() << std::endl;
	}

	return (0);
}