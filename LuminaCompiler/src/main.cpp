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

struct NumberExpressionValueInstruction : public AbstractInstruction
{
	const Lumina::Token& token;

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

struct ConditionInstruction : public AbstractInstruction
{
	std::vector<std::shared_ptr<ExpressionInstruction>> elements;

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

	const Lumina::Token& nextToken() const
	{
		return (_tokens->operator[](_index + 1));
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

	std::shared_ptr<IncludeInstruction> parseIncludeInstruction()
	{
		std::shared_ptr<IncludeInstruction> result = std::make_shared<IncludeInstruction>();

		expect(Lumina::Token::Type::Include);
		result->includeFile = expect({ Lumina::Token::Type::StringLitteral, Lumina::Token::Type::IncludeLitteral }, "Expected a valid include file token.");

		return (result);
	}

	std::shared_ptr<TypeInstruction> parseTypeInstruction()
	{
		std::shared_ptr<TypeInstruction> result = std::make_shared<TypeInstruction>();

		result->tokens.push_back(expect(Lumina::Token::Type::Identifier));
		while (currentToken().type == Lumina::Token::Type::NamespaceSeparator)
		{
			result->tokens.push_back(expect(Lumina::Token::Type::NamespaceSeparator));
			result->tokens.push_back(expect(Lumina::Token::Type::Identifier));
		}

		return (result);
	}

	std::shared_ptr<IdentifierInstruction> parseIdentifierInstruction()
	{
		std::shared_ptr<IdentifierInstruction> result = std::make_shared<IdentifierInstruction>();

		result->token = expect(Lumina::Token::Type::Identifier);

		return (result);
	}

	std::shared_ptr<PipelineFlowInstruction> parsePipelineFlowInstruction()
	{
		std::shared_ptr<PipelineFlowInstruction> result = std::make_shared<PipelineFlowInstruction>();

		result->inputPipeline = expect(Lumina::Token::Type::PipelineFlow);
		expect(Lumina::Token::Type::PipelineFlowSeparator);
		result->outputPipeline = expect(Lumina::Token::Type::PipelineFlow);
		expect(Lumina::Token::Type::Separator);
		result->type = parseTypeInstruction();
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<BlockElementInstruction> parseBlockElementInstruction()
	{
		std::shared_ptr<BlockElementInstruction> result = std::make_shared<BlockElementInstruction>();

		result->type = parseTypeInstruction();
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<StructureBlockInstruction> parseStructureBlockInstruction()
	{
		std::shared_ptr<StructureBlockInstruction> result = std::make_shared<StructureBlockInstruction>();

		expect(Lumina::Token::Type::StructureBlock);
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::OpenCurlyBracket);
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction());
		}
		expect(Lumina::Token::Type::CloseCurlyBracket);
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<AttributeBlockInstruction> parseAttributeBlockInstruction()
	{
		std::shared_ptr<AttributeBlockInstruction> result = std::make_shared<AttributeBlockInstruction>();

		expect(Lumina::Token::Type::AttributeBlock);
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::OpenCurlyBracket);
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			result->elements.push_back(parseBlockElementInstruction());
		}
		expect(Lumina::Token::Type::CloseCurlyBracket);
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<ConstantBlockInstruction> parseConstantBlockInstruction()
	{
		std::shared_ptr<ConstantBlockInstruction> result = std::make_shared<ConstantBlockInstruction>();

		expect(Lumina::Token::Type::ConstantBlock);
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::OpenCurlyBracket);
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				result->elements.push_back(parseBlockElementInstruction());
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);

				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket);
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<TextureInstruction> parseTextureInstruction()
	{
		std::shared_ptr<TextureInstruction> result = std::make_shared<TextureInstruction>();

		expect(Lumina::Token::Type::Texture);
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::EndOfSentence);

		return (result);
	}

	std::shared_ptr<SymbolParameterInstruction> parseSymbolParameterInstruction()
	{
		std::shared_ptr<SymbolParameterInstruction> result = std::make_shared<SymbolParameterInstruction>();

		result->type = parseTypeInstruction();
		result->name = parseIdentifierInstruction();

		return (result);
	}

	std::shared_ptr<SymbolBodyInstruction> parseSymbolBodyInstruction()
	{
		std::shared_ptr<SymbolBodyInstruction> result = std::make_shared<SymbolBodyInstruction>();

		expect(Lumina::Token::Type::OpenCurlyBracket);
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
		{
			try
			{
				switch (currentToken().type)
				{
				case Lumina::Token::Type::Comment:
					skipToken();
					break;
				default:
					throw Lumina::TokenBasedError(_file, "Unexpected token type: " + to_string(currentToken().type), currentToken());
				}
			}
			catch (const Lumina::TokenBasedError& e)
			{
				_result.errors.push_back(e);
				skipLine();
			}
		}
		expect(Lumina::Token::Type::CloseCurlyBracket);

		return result;
	}

	std::shared_ptr<SymbolInstruction> parseSymbolInstruction()
	{
		std::shared_ptr<SymbolInstruction> result = std::make_shared<SymbolInstruction>();

		result->returnType = parseTypeInstruction();
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::OpenParenthesis);
		while (currentToken().type != Lumina::Token::Type::CloseParenthesis)
		{
			result->parameters.push_back(parseSymbolParameterInstruction());
			if (currentToken().type != Lumina::Token::Type::CloseParenthesis)
				expect(Lumina::Token::Type::Comma);
		}
		expect(Lumina::Token::Type::CloseParenthesis);
		result->body = parseSymbolBodyInstruction();

		return (result);
	}

	std::shared_ptr<NamespaceInstruction> parseNamespaceInstruction()
	{
		std::shared_ptr<NamespaceInstruction> result = std::make_shared<NamespaceInstruction>();

		expect(Lumina::Token::Type::Namespace);
		result->name = parseIdentifierInstruction();
		expect(Lumina::Token::Type::OpenCurlyBracket);
		while (currentToken().type != Lumina::Token::Type::CloseCurlyBracket)
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
				case Lumina::Token::Type::StructureBlock:
				{
					result->instructions.push_back(parseStructureBlockInstruction());
					break;
				}
				case Lumina::Token::Type::AttributeBlock:
				{
					result->instructions.push_back(parseAttributeBlockInstruction());
					break;
				}
				case Lumina::Token::Type::ConstantBlock:
				{
					result->instructions.push_back(parseConstantBlockInstruction());
					break;
				}
				case Lumina::Token::Type::Texture:
				{
					result->instructions.push_back(parseTextureInstruction());
					break;
				}
				case Lumina::Token::Type::Namespace:
				{
					result->instructions.push_back(parseNamespaceInstruction());
					break;
				}
				case Lumina::Token::Type::Identifier:
				{
					result->instructions.push_back(parseSymbolInstruction());
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
		expect(Lumina::Token::Type::CloseCurlyBracket);

		return (result);
	}

	std::shared_ptr<PipelineBodyInstruction> parsePipelineBodyInstruction()
	{
		std::shared_ptr<PipelineBodyInstruction> result = std::make_shared<PipelineBodyInstruction>();

		result->pipelineToken = expect(Lumina::Token::Type::PipelineFlow);
		expect(Lumina::Token::Type::OpenParenthesis);
		expect(Lumina::Token::Type::CloseParenthesis);

		result->body = parseSymbolBodyInstruction();

		return (result);
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