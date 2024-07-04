#include "utils/spk_string_utils.hpp"

namespace spk
{
	namespace StringUtils
	{
		std::wstring stringToWString(const std::string& p_str)
		{
			return std::wstring(p_str.begin(), p_str.end());
		}
	}
}