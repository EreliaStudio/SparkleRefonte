#include "lumina_error.hpp"

#include <sstream>
#include <iomanip>

namespace Lumina
{
	CompilationError::CompilationError(const std::string& p_message, const std::string& p_file, size_t p_lineIndex, const std::string& p_line, size_t p_column, size_t p_tokenSize) :
		_file(p_file),
		_lineIndex(p_lineIndex),
		_line(p_line),
		_column(p_column),
		_tokenSize(p_tokenSize),
		_message(p_message)
	{
		std::stringstream ss;

		size_t subTokenSize = 0;
		if (p_tokenSize > 2)
			subTokenSize = p_tokenSize - 2;

		ss << "In file [" << _file << "] : " << std::endl;
		ss << "    Error on line[" << std::setw(5) << (_lineIndex + 1) << "] : " << _message << std::endl;
		if (p_tokenSize == 0)
		{

		}
		else if (p_tokenSize == 1)
		{
			ss << std::string(14, ' ') << _line << std::endl;
			ss << std::string(14, ' ') << std::string(_column, ' ') << '|';
		}
		else
		{
			ss << std::string(14, ' ') << _line << std::endl;
			ss << std::string(14, ' ') << std::string(_column, ' ') << '|' << (subTokenSize != 0 ? std::string(subTokenSize, L'-') : "") << '|';
		}

		_what = ss.str();
	}

	size_t CompilationError::lineIndex() const
	{
		return (_lineIndex);
	}

	const std::string& CompilationError::line() const
	{
		return (_line);
	}

	size_t CompilationError::column() const
	{
		return (_column);
	}

	size_t CompilationError::tokenSize() const
	{
		return (_tokenSize);
	}

	const std::string& CompilationError::message() const
	{
		return (_message);
	}

	const char* CompilationError::what() const
	{
		return (_what.c_str());
	}

	TokenBasedError::TokenBasedError(const std::string& p_message, const Tokenizer::Token& p_token) :
		_message(p_message),
		_token(p_token)
	{

	}

	const char* TokenBasedError::what() const noexcept
	{
		return _message.c_str();
	}

	const Tokenizer::Token& TokenBasedError::token() const noexcept
	{
		return _token;
	}
}