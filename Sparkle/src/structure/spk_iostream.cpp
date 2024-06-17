#include "structure/spk_iostream.hpp"

namespace spk
{
	IOStream::IOStream(std::wostream& p_stream) : _stream(&p_stream), _newLine(true)
	{
	}

	void IOStream::setPrefix(const std::wstring& p_prefix)
	{
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		_prefix = p_prefix;
	}

	// To support std::endl and other manipulators
	IOStream& IOStream::operator<<(CoutType& (*p_func)(CoutType&))
	{
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		if (_newLine)
		{
			*_stream << _prefix;
			_newLine = false;
		}
		*_stream << _buffer.str();
		_buffer.str(L"");
		_buffer.clear();
		p_func(*_stream);
		_newLine = true;
		return *this;
	}

	void IOStream::redirect(std::wostream& p_stream)
	{
		std::lock_guard<std::recursive_mutex> lock(_mutex);
		_stream = &p_stream;
	}

	spk::IOStream cout = spk::IOStream(std::wcout);
	spk::IOStream cerr = spk::IOStream(std::wcerr);
}