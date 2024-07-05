#pragma once

#include <exception>
#include <string>

namespace Lumina
{
	class CompilationError : public std::exception
	{
	private:
		size_t _lineIndex;
		std::string _line;
		size_t _column;
		size_t _tokenSize;
		std::string _message;
		std::string _what;

	public:
		CompilationError(const std::string& p_message, size_t p_lineIndex, const std::string& p_line, size_t p_column, size_t p_tokenSize);
		size_t lineIndex() const;
		const std::string& line() const;
		size_t column() const;
		size_t tokenSize() const;
		const std::string& message() const;
		const char* what() const;
	};
}