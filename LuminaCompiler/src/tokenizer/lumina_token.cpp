#include "lumina_tokenizer.hpp"

#include <iomanip>

namespace Lumina
{
	std::ostream& operator << (std::ostream& p_os, const Tokenizer::Token::Type& p_type)
	{
		p_os << to_string(p_type);
		return (p_os);
	}

	std::ostream& operator << (std::ostream& p_os, const Tokenizer::Token& p_token)
	{
		p_os << "[" << std::setw(25) << p_token.type << "] - Line [" << std::setw(4) << p_token.line + 1 << "] - Column [" << std::setw(4) << p_token.column << "] - [" << p_token.content << "]";
		return (p_os);
	}
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type)
{
	switch (p_type)
	{
		case Lumina::Tokenizer::Token::Type::SingleLineComment:
			return ("SingleLineComment");
		case Lumina::Tokenizer::Token::Type::MultiLineCommentStart:
			return ("MultiLineCommentStart");
		case Lumina::Tokenizer::Token::Type::MultiLineCommentStop:
			return ("MultiLineCommentStop");
		case Lumina::Tokenizer::Token::Type::Import:
			return ("Import");
		case Lumina::Tokenizer::Token::Type::StringLitterals:
			return ("StringLitterals");
		case Lumina::Tokenizer::Token::Type::PipelineFlow:
			return ("PipelineFlow");
		case Lumina::Tokenizer::Token::Type::PipelineSeparator:
			return ("PipelineSeparator");
		case Lumina::Tokenizer::Token::Type::Separator:
			return ("Separator");
		case Lumina::Tokenizer::Token::Type::Structure:
			return ("Structure");
		case Lumina::Tokenizer::Token::Type::Identifier:
			return ("Identifier");
		case Lumina::Tokenizer::Token::Type::Texture:
			return ("Texture");
		case Lumina::Tokenizer::Token::Type::AttributeBlock:
			return ("AttributeBlock");
		case Lumina::Tokenizer::Token::Type::ConstantBlock:
			return ("ConstantBlock");
		case Lumina::Tokenizer::Token::Type::Assignator:
			return ("Assignator");
		case Lumina::Tokenizer::Token::Type::Accessor:
			return ("Accessor");
		case Lumina::Tokenizer::Token::Type::Symbol:
			return ("Symbol");
		case Lumina::Tokenizer::Token::Type::OpenParenthesis:
			return ("OpenParenthesis");
		case Lumina::Tokenizer::Token::Type::ClosedParenthesis:
			return ("ClosedParenthesis");
		case Lumina::Tokenizer::Token::Type::Comma:
			return ("Comma");
		case Lumina::Tokenizer::Token::Type::Operator:
			return ("Operator");
		case Lumina::Tokenizer::Token::Type::EndOfSentence:
			return ("EndOfSentence");
		case Lumina::Tokenizer::Token::Type::Number:
			return ("Number");
		case Lumina::Tokenizer::Token::Type::Bool:
			return ("Bool");
		case Lumina::Tokenizer::Token::Type::Namespace:
			return ("Namespace");
		case Lumina::Tokenizer::Token::Type::NamespaceSeparator:
			return ("NamespaceSeparator");
		case Lumina::Tokenizer::Token::Type::OpenCurlyBracket:
			return ("OpenCurlyBracket");
		case Lumina::Tokenizer::Token::Type::ClosedCurlyBracket:
			return ("ClosedCurlyBracket");
		case Lumina::Tokenizer::Token::Type::IfStatement:
			return ("IfStatement");
		case Lumina::Tokenizer::Token::Type::WhileStatement:
			return ("WhileStatement");
		case Lumina::Tokenizer::Token::Type::ForStatement:
			return ("ForStatement");
		case Lumina::Tokenizer::Token::Type::SwitchStatement:
			return ("SwitchStatement");
		case Lumina::Tokenizer::Token::Type::Case:
			return ("Case");
		case Lumina::Tokenizer::Token::Type::Break:
			return ("Break");
		case Lumina::Tokenizer::Token::Type::Return:
			return ("Return");
		case Lumina::Tokenizer::Token::Type::Discard:
			return ("Discard");
		case Lumina::Tokenizer::Token::Type::Unknown:
			return ("Unknown");
		case Lumina::Tokenizer::Token::Type::Type:
			return ("Type");
		case Lumina::Tokenizer::Token::Type::Name:
			return ("Name");
		case Lumina::Tokenizer::Token::Type::SymbolCall:
			return ("SymbolCall");
		case Lumina::Tokenizer::Token::Type::MetaToken:
			return ("MetaToken");
	
	default:
		return ("InvalidType");
	}
}