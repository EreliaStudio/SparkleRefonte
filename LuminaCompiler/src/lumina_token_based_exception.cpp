#include "lumina_exception.hpp"

namespace Lumina
{
	TokenBasedError::TokenBasedError(const std::filesystem::path& p_file, const std::string& p_message, const Token& p_token)
	{
		size_t tokenSize = p_token.content.size();
		std::stringstream ss;

		ss << "In file [" << p_file.string() << "] : " << std::endl;
		ss << "    Error on line[" << std::setw(5) << (p_token.context.line + 1) << "] : " << p_message << std::endl;
		if (p_token.context.line == -1 || tokenSize == 0)
		{

		}
		else if (tokenSize == 1)
		{
			ss << std::string(14, ' ') << p_token.context.inputLine << std::endl;
			if (p_token.context.column != -1)
				ss << std::string(14, ' ') << std::string(p_token.context.column, ' ') << '|';
		}
		else
		{
			ss << std::string(14, ' ') << p_token.context.inputLine << std::endl;
			if (p_token.context.column != -1)
				ss << std::string(14, ' ') << std::string(p_token.context.column, ' ') << '|' << std::string(tokenSize - 2, '-') << '|';
		}

		_what = ss.str();
	}

	const char* TokenBasedError::what() const noexcept
	{
		return (_what.c_str());
	}
}