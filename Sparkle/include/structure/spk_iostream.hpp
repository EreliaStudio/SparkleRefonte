#pragma once

#include <iostream>
#include <sstream>
#include <mutex>

namespace spk
{
	class IOStream
	{
	private:
		std::wostream* _stream;
		std::wstringstream _buffer;
		std::wstring _prefix;
		std::recursive_mutex _mutex;
		bool _newLine;

	public:
		IOStream(std::wostream& stream);

		void setPrefix(const std::wstring& prefix);

		template <typename TOutputedType>
		IOStream& operator<<(const TOutputedType& value)
		{
			std::lock_guard<std::recursive_mutex> lock(_mutex);
			if (_newLine)
			{
				*_stream << _prefix;
				_newLine = false;
			}
			_buffer << value;

			std::wstring bufferContent = _buffer.str();
			size_t pos = 0;

			while ((pos = bufferContent.find(L'\n', pos)) != std::wstring::npos)
			{
				_newLine = true;
				pos++;
			}

			*_stream << _buffer.str();
			_buffer.str(L"");
			_buffer.clear();
			return *this;
		}

		typedef std::basic_ostream<wchar_t, std::char_traits<wchar_t>> CoutType;

		// To support std::endl and other manipulators
		IOStream& operator<<(CoutType& (*func)(CoutType&));

		void redirect(std::wostream& p_stream);
	};

	extern IOStream cout;
	extern IOStream cerr;
}