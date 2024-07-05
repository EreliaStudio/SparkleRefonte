#include "lumina_tokenizer.hpp"

#include <iomanip>

namespace Lumina
{
	std::ostream& operator << (std::ostream& p_os, const Tokenizer::Token::Type& p_type)
	{
		p_os << to_string(p_type);
		return (p_os);
	}

	std::wostream& operator << (std::wostream& p_os, const Tokenizer::Token::Type& p_type)
	{
		p_os << to_wstring(p_type);
		return (p_os);
	}

	std::ostream& operator << (std::ostream& p_os, const Tokenizer::Token& p_token)
	{
		p_os << "[" << std::setw(25) << p_token.type << "] - Line [" << std::setw(4) << p_token.line << "] - Column [" << std::setw(4) << p_token.column << "] - " << p_token.content;
		return (p_os);
	}
}

std::string to_string(Lumina::Tokenizer::Token::Type p_type)
{
	switch (p_type)
	{
	case Lumina::Tokenizer::Token::Type::Include:
		return "Include";
	case Lumina::Tokenizer::Token::Type::PipelineFlow:
		return "PipelineFlow";
	case Lumina::Tokenizer::Token::Type::PipelineFlowSeparator:
		return "PipelineFlowSeparator";
	case Lumina::Tokenizer::Token::Type::NamespaceSeparator:
		return "NamespaceSeparator";
	case Lumina::Tokenizer::Token::Type::Keyword:
		return "Keyword";
	case Lumina::Tokenizer::Token::Type::Identifier:
		return "Identifier";
	case Lumina::Tokenizer::Token::Type::Number:
		return "Number";
	case Lumina::Tokenizer::Token::Type::StringLiteral:
		return "StringLiteral";
	case Lumina::Tokenizer::Token::Type::Assignator:
		return "Assignator";
	case Lumina::Tokenizer::Token::Type::Operator:
		return "Operator";
	case Lumina::Tokenizer::Token::Type::Separator:
		return "Separator";
	case Lumina::Tokenizer::Token::Type::BodyOpener:
		return "BodyOpener";
	case Lumina::Tokenizer::Token::Type::BodyCloser:
		return "BodyCloser";
	case Lumina::Tokenizer::Token::Type::ParenthesisOpener:
		return "ParenthesisOpener";
	case Lumina::Tokenizer::Token::Type::ParenthesisCloser:
		return "ParenthesisCloser";
	case Lumina::Tokenizer::Token::Type::InstructionEnd:
		return "InstructionEnd";
	case Lumina::Tokenizer::Token::Type::Accessor:
		return "Accessor";
	case Lumina::Tokenizer::Token::Type::Comment:
		return "Comment";
	case Lumina::Tokenizer::Token::Type::SingleLineComment:
		return "SingleLineComment";
	case Lumina::Tokenizer::Token::Type::MultilineComment:
		return "MultilineComment";
	case Lumina::Tokenizer::Token::Type::EndOfMultilineComment:
		return "EndOfMultilineComment";
	case Lumina::Tokenizer::Token::Type::EndOfSentence:
		return "EndOfSentence";
	case Lumina::Tokenizer::Token::Type::Unknown:
		return "Unknown";
	default:
		return "InvalidType";
	}
}

std::wstring to_wstring(Lumina::Tokenizer::Token::Type p_type)
{
	switch (p_type)
	{
	case Lumina::Tokenizer::Token::Type::Include:
		return L"Include";
	case Lumina::Tokenizer::Token::Type::PipelineFlow:
		return L"PipelineFlow";
	case Lumina::Tokenizer::Token::Type::PipelineFlowSeparator:
		return L"PipelineFlowSeparator";
	case Lumina::Tokenizer::Token::Type::NamespaceSeparator:
		return L"NamespaceSeparator";
	case Lumina::Tokenizer::Token::Type::Keyword:
		return L"Keyword";
	case Lumina::Tokenizer::Token::Type::Identifier:
		return L"Identifier";
	case Lumina::Tokenizer::Token::Type::Number:
		return L"Number";
	case Lumina::Tokenizer::Token::Type::StringLiteral:
		return L"StringLiteral";
	case Lumina::Tokenizer::Token::Type::Assignator:
		return L"Assignator";
	case Lumina::Tokenizer::Token::Type::Operator:
		return L"Operator";
	case Lumina::Tokenizer::Token::Type::Separator:
		return L"Separator";
	case Lumina::Tokenizer::Token::Type::BodyOpener:
		return L"BodyOpener";
	case Lumina::Tokenizer::Token::Type::BodyCloser:
		return L"BodyCloser";
	case Lumina::Tokenizer::Token::Type::ParenthesisOpener:
		return L"ParenthesisOpener";
	case Lumina::Tokenizer::Token::Type::ParenthesisCloser:
		return L"ParenthesisCloser";
	case Lumina::Tokenizer::Token::Type::InstructionEnd:
		return L"InstructionEnd";
	case Lumina::Tokenizer::Token::Type::Accessor:
		return L"Accessor";
	case Lumina::Tokenizer::Token::Type::Comment:
		return L"Comment";
	case Lumina::Tokenizer::Token::Type::SingleLineComment:
		return L"SingleLineComment";
	case Lumina::Tokenizer::Token::Type::MultilineComment:
		return L"MultilineComment";
	case Lumina::Tokenizer::Token::Type::EndOfMultilineComment:
		return L"EndOfMultilineComment";
	case Lumina::Tokenizer::Token::Type::EndOfSentence:
		return L"EndOfSentence";
	case Lumina::Tokenizer::Token::Type::Unknown:
		return L"Unknown";
	default:
		return L"InvalidType";
	}
}