#pragma once

#include <exception>
#include <string>
#include <sstream>

namespace spk
{
	class CompilationError : public std::exception
	{
	private:
		int _lineIndex;
		std::wstring _line;
		size_t _column;
		size_t _tokenSize;
		std::wstring _message;
		std::wstring _what;

	public:
		CompilationError(const std::wstring& p_message, int p_lineIndex, const std::wstring& p_line, size_t p_column, size_t p_tokenSize);
		int lineIndex() const;
		const std::wstring& line() const;
		size_t column() const;
		size_t tokenSize() const;
		const std::wstring& message() const;
		const char * what() const;
	};
}