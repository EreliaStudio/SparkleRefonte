#include "lumina_parser.hpp"

namespace Lumina
{
	void Parser::insertError(const std::string& p_error, const Tokenizer::Token& p_incriminedToken)
	{
		_result.errors.push_back(CompilationError(p_error, p_incriminedToken.line, p_incriminedToken.fullLine, p_incriminedToken.column, p_incriminedToken.content.size()));
	}

	bool Parser::hasInstructionLeft()
	{
		return (_index < _instructions.size());
	}

	const Lexer::Instruction& Parser::currentInstruction() const
	{
		return (_instructions[_index]);
	}

	void Parser::advance()
	{
		_index++;
	}

	Parser::Result Parser::validate(const std::vector<Lexer::Instruction>& p_instructions)
	{
		return (Parser().execute(p_instructions));
	}
}