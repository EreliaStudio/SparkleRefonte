#include "lumina_token.hpp"

namespace Lumina
{
	Token::Token(const std::string& p_content, Type p_type, int p_line, int p_column, const std::string& p_inputLine) :
		type(p_type),
		content(p_content),
		context{ p_line, p_column, p_inputLine }
	{
	}

	std::ostream& operator << (std::ostream& p_os, const Token::Type& p_type)
	{
		p_os << to_string(p_type);
		return p_os;
	}

	std::ostream& operator << (std::ostream& p_os, const Token& p_token)
	{
		p_os << "[" << std::setw(25) << p_token.type << "] | [" << std::setw(3) << p_token.context.line << "::" << std::left << std::setw(3) << p_token.context.column << std::right << "] | " << p_token.content;

		return (p_os);
	}

	std::string to_string(Token::Type p_type)
	{
		switch (p_type)
		{
		case Token::Type::Unknow: return "Unknow";
		case Token::Type::Include: return "#include";
		case Token::Type::StringLitteral: return "StringLitteral";
		case Token::Type::IncludeLitteral: return "IncludeLitteral";
		case Token::Type::PipelineFlow: return "PipelineFlow";
		case Token::Type::PipelineFlowSeparator: return "PipelineFlowSeparator";
		case Token::Type::NamespaceSeparator: return "NamespaceSeparator";
		case Token::Type::Separator: return "Separator";
		case Token::Type::Identifier: return "Identifier";
		case Token::Type::Number: return "Number";
		case Token::Type::StructureBlock: return "StructureBlock";
		case Token::Type::AttributeBlock: return "AttributeBlock";
		case Token::Type::ConstantBlock: return "ConstantBlock";
		case Token::Type::Texture: return "Texture";
		case Token::Type::Namespace: return "Namespace";
		case Token::Type::OpenCurlyBracket: return "OpenCurlyBracket";
		case Token::Type::CloseCurlyBracket: return "CloseCurlyBracket";
		case Token::Type::OpenParenthesis: return "OpenParenthesis";
		case Token::Type::CloseParenthesis: return "CloseParenthesis";
		case Token::Type::Accessor: return "Accessor";
		case Token::Type::Comment: return "Comment";
		case Token::Type::Operator: return "Operator";
		case Token::Type::Return: return "Return";
		case Token::Type::Discard: return "Discard";
		case Token::Type::IfStatement: return "IfStatement";
		case Token::Type::WhileStatement: return "WhileStatement";
		case Token::Type::ForStatement: return "ForStatement";
		case Token::Type::EndOfSentence: return "EndOfSentence";
		case Token::Type::Assignator: return "Assignator";
		case Token::Type::Comma: return "Comma";
		default: return "Unknown";
		}
	}
}