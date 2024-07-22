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
		switch (p_type)
		{
		case Token::Type::Unknow: p_os << "Unknow    "; break;
		case Token::Type::Include: p_os << "Include"; break;
		case Token::Type::StringLitteral: p_os << "StringLitteral"; break;
		case Token::Type::IncludeLitteral: p_os << "IncludeLitteral"; break;
		case Token::Type::PipelineFlow: p_os << "PipelineFlow"; break;
		case Token::Type::PipelineFlowSeparator: p_os << "PipelineFlowSeparator"; break;
		case Token::Type::Separator: p_os << "Separator"; break;
		case Token::Type::Identifier: p_os << "Identifier"; break;
		case Token::Type::Number: p_os << "Number"; break;
		case Token::Type::StructureBlock: p_os << "StructureBlock"; break;
		case Token::Type::AttributeBlock: p_os << "AttributeBlock"; break;
		case Token::Type::ConstantBlock: p_os << "ConstantBlock"; break;
		case Token::Type::Texture: p_os << "Texture"; break;
		case Token::Type::Namespace: p_os << "Namespace"; break;
		case Token::Type::NamespaceSeparator: p_os << "NamespaceSeparator"; break;
		case Token::Type::OpenCurlyBracket: p_os << "OpenCurlyBracket"; break;
		case Token::Type::CloseCurlyBracket: p_os << "CloseCurlyBracket"; break;
		case Token::Type::OpenParenthesis: p_os << "OpenParenthesis"; break;
		case Token::Type::CloseParenthesis: p_os << "CloseParenthesis"; break;
		case Token::Type::Accessor: p_os << "Accessor"; break;
		case Token::Type::Comment: p_os << "Comment"; break;
		case Token::Type::Operator: p_os << "Operator"; break;
		case Token::Type::Return: p_os << "Return"; break;
		case Token::Type::Discard: p_os << "Discard"; break;
		case Token::Type::IfStatement: p_os << "IfStatement"; break;
		case Token::Type::WhileStatement: p_os << "WhileStatement"; break;
		case Token::Type::ForStatement: p_os << "ForStatement"; break;
		case Token::Type::EndOfSentence: p_os << "EndOfSentence"; break;
		case Token::Type::Assignator: p_os << "Assignator"; break;
		case Token::Type::Comma: p_os << "Comma"; break;
		default: p_os << "Invalid token type"; break;
		}
		return p_os;
	}

	std::ostream& operator << (std::ostream& p_os, const Token& p_token)
	{
		p_os << "[" << std::setw(25) << p_token.type << "] | [" << std::setw(3) << p_token.context.line << "::" << std::left << std::setw(3) << p_token.context.column << std::right << "] | " << p_token.content;

		return (p_os);
	}
}