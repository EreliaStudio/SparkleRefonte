#include "structure/spk_iostream.hpp"

namespace spk
{
	thread_local spk::IOStream cout(std::wcout);
	thread_local spk::IOStream cerr(std::wcerr);

	void IOStream::_flushBuffer()
	{
		if (_outputStream == nullptr)
			_outputStream = &std::wcout;
		auto bufferContent = _buffer.str();

		if (bufferContent.size() == 0)
			return;

		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_prefix != L"")
				*_outputStream << "[" << _prefix << "] - ";
			*_outputStream << bufferContent;
		}
		_buffer.str(L"");
		_buffer.clear();
	}

	IOStream::IOStream(std::wostream& p_outputStream) :
		_outputStream(&p_outputStream),
		_prefix(L"")
	{

	}

	void IOStream::setPrefix(const std::wstring& p_prefix)
	{
		_prefix = p_prefix;
	}

	void IOStream::redirect(std::wostream& p_outputStream)
	{
		_outputStream = &p_outputStream;
	}

	void IOStream::flush()
	{
		_flushBuffer();
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

	std::wstring IOStream::str() const
	{
		return (_buffer.str());
	}
}