#pragma once

#include <iostream>
#include <sstream>
#include <mutex>

namespace spk
{
    class IOStream
    {
    public:
        IOStream(std::wostream& stream) :
            _stream(stream)
        {
        }

        template<typename T>
        IOStream& operator<<(const T& value)
        {
            //std::lock_guard<std::mutex> lock(_mutex);
            _stream << value;
            return *this;
        }

        // For manipulators like std::endl
        IOStream& operator<<(std::wostream& (*manip)(std::wostream&))
        {
            //std::lock_guard<std::mutex> lock(_mutex);
            _stream << manip;
            return *this;
        }

    private:
        std::wostream& _stream;
        std::mutex _mutex;
    };

	extern IOStream cout;
	extern IOStream cerr;
}