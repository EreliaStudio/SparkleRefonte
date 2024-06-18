#include "structure/spk_iostream.hpp"

namespace spk
{
	spk::IOStream cout = spk::IOStream(std::wcout);
	spk::IOStream cerr = spk::IOStream(std::wcerr);
}