#pragma once

#include <iostream>
#include <sstream>
#include <mutex>

namespace spk
{
	class IOStream
	{
	private:
		std::wostream* _outputStream = nullptr;
		static inline std::recursive_mutex _mutex;
		std::wstringstream _buffer;
		std::wstring _prefix;

		void _flushBuffer();

	public:
		IOStream(std::wostream& p_outputStream);
		void setPrefix(const std::wstring& p_prefix);

		void redirect(std::wostream& p_outputStream);

		void flush();

		template <typename T>
		IOStream& operator<<(const T& value)
		{
			_buffer << value;
			return *this;
		}

		using Manipulator = std::wostream& (std::wostream&);

		IOStream& operator<<(Manipulator manip);

		std::wstring str() const;
	};

	extern thread_local IOStream cout;
	extern thread_local IOStream cerr;
}
