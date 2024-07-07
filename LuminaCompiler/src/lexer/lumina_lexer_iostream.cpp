#include "lumina_lexer.hpp"

#include <iomanip>

namespace Lumina
{
	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction::Type& p_type)
	{
		p_os << to_string(p_type);
		return p_os;
	}

	std::ostream& operator << (std::ostream& p_os, const Lexer::Instruction& p_instruction)
	{
		p_os << "Type [" << std::setw(15) << p_instruction.type << "] - ";
		if (p_instruction._tokens.size() == 0)
		{
			p_os << "No token";
			return (p_os);
		}

		size_t currentLine = p_instruction._tokens[0].line;
		for (size_t i = 0; i < p_instruction._tokens.size(); i++)
		{
			if (p_instruction._tokens[i].line != currentLine)
			{
				p_os << "\n";
				currentLine = p_instruction._tokens[i].line;
			}
			if (i != 0)
				p_os << " ";
			p_os << p_instruction._tokens[i].content;
		}

		return (p_os);
	}
}

std::string to_string(Lumina::Lexer::Instruction::Type p_type)
{
	switch (p_type)
	{
	case Lumina::Lexer::Instruction::Type::Include:
		return "Include";
	case Lumina::Lexer::Instruction::Type::PipelineFlow:
		return "PipelineFlow";
	case Lumina::Lexer::Instruction::Type::Structure:
		return "Structure";
	case Lumina::Lexer::Instruction::Type::Function:
		return "Function";
	case Lumina::Lexer::Instruction::Type::AttributeBlock:
		return "AttributeBlock";
	case Lumina::Lexer::Instruction::Type::ConstantBlock:
		return "ConstantBlock";
	case Lumina::Lexer::Instruction::Type::Texture:
		return "Texture";
	case Lumina::Lexer::Instruction::Type::Namespace:
		return "Namespace";
	case Lumina::Lexer::Instruction::Type::Comment:
		return "Comment";
	case Lumina::Lexer::Instruction::Type::PipelinePass:
		return "PipelinePass";
	case Lumina::Lexer::Instruction::Type::Body:
		return "Body";
	case Lumina::Lexer::Instruction::Type::Member:
		return "Member";
	case Lumina::Lexer::Instruction::Type::RValue:
		return "RValue";
	case Lumina::Lexer::Instruction::Type::Expression:
		return "Expression";
	case Lumina::Lexer::Instruction::Type::Symbol:
		return "Symbol";
	case Lumina::Lexer::Instruction::Type::Parameter:
		return "Parameter";
		
	default:
		return "Unknown";
	}
}