#pragma once

#include "spk_define.hpp"

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
		spk::String _prefix;
		std::recursive_mutex _mutex;
		bool _newLine;

	public:
		IOStream(std::wostream& stream);

		void setPrefix(const spk::String& prefix);

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

			spk::String bufferContent = _buffer.str();
			spk::Lenght pos = 0;

			while ((pos = bufferContent.find(L'\n', pos)) != spk::String::npos)
			{
				_newLine = true;
				pos++;
			}

			*_stream << _buffer.str();
			_buffer.str(L"");
			_buffer.clear();
			return *this;
		}

		typedef std::basic_ostream<spk::Char, std::char_traits<spk::Char>> CoutType;

		// To support std::endl and other manipulators
		IOStream& operator<<(CoutType& (*func)(CoutType&));

		void redirect(std::wostream& p_stream);
	};

	extern IOStream cout;
	extern IOStream cerr;
}