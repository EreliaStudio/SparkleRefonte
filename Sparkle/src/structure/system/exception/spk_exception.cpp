#include "structure/system/exception/spk_exception.hpp"

#include <iomanip>

namespace spk
{
	CompilationError::CompilationError(const std::wstring& p_message, int p_lineIndex, const std::wstring& p_line, size_t p_column, size_t p_tokenSize) :
		_lineIndex(p_lineIndex),
		_line(p_line),
		_column(p_column),
		_tokenSize(p_tokenSize),
		_message(p_message)
	{
		std::wstringstream woss;
		
		size_t subTokenSize = 0;
		if (p_tokenSize > 2)
			subTokenSize = p_tokenSize - 2;

		woss << L"Error on line [" << std::setw(5) << _lineIndex << "] : " << _message << std::endl;
		woss << std::wstring(L' ', 10) << _line << std::endl;
		woss << std::wstring(L' ', 10) << std::wstring(L' ', _column) << L'|' << (subTokenSize != 0 ? std::wstring(L'-', subTokenSize) : L"") << L'|';

		_what = woss.str();
	}

	int CompilationError::lineIndex() const
	{
		return (_lineIndex);
	}
	
	const std::wstring& CompilationError::line() const
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
	
	const std::wstring& CompilationError::message() const
	{
		return (_message);
	}
	
	const char* CompilationError::what() const
	{
		return (reinterpret_cast<const char *>(_what.c_str()));
	}
}