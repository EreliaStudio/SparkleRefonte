#include "structure/spk_iostream.hpp"

namespace spk
{
	void IOStream::_flushBuffer()
	{
		auto bufferContent = _buffer.str();
		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_prefix != L"")
				_outputStream << "[" << _prefix << "] - ";
			_outputStream << bufferContent;
		}
		_buffer.str(L"");
		_buffer.clear();
	}

	IOStream::IOStream(std::wostream& p_outputStream) :
		_outputStream(p_outputStream),
		_prefix(L"")
	{

	}

	void IOStream::setPrefix(const std::wstring& p_prefix)
	{
		_prefix = p_prefix;
	}

	IOStream& IOStream::operator<<(Manipulator manip)
	{
		_buffer << manip;
		if (manip == static_cast<Manipulator&>(std::endl))
		{
			_flushBuffer();
		}
		return *this;
	}

	thread_local spk::IOStream cout = spk::IOStream(std::wcout);
	thread_local spk::IOStream cerr = spk::IOStream(std::wcerr);
}