#pragma once

#include <exception>
#include <string>
#include <sstream>

class PipelineException : public std::exception
{
public:
	PipelineException(int line, const std::string& functionName, const std::string& message) :
		_line(line),
		_functionName(functionName),
		_message(message)
	{
		std::ostringstream oss;
		oss << _functionName << "::" << _line << " - " << _message;
		_fullMessage = oss.str();
	}

	int getLine() const
	{
		return _line;
	}

	const std::string& getFunctionName() const
	{
		return _functionName;
	}

	const std::string& getMessage() const
	{
		return _message;
	}

	const char* what() const noexcept override
	{
		return _fullMessage.c_str();
	}

private:
	int _line;
	std::string _functionName;
	std::string _message;
	std::string _fullMessage;
};