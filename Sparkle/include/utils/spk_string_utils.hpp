#pragma once

#include <string>
#include <vector>

namespace spk
{
	namespace StringUtils
	{
		std::wstring stringToWString(const std::string& p_str);
		std::string wstringToString(const std::wstring& p_wstr);

		std::vector<std::string> splitString(const std::string& p_str, const std::string& p_delimiter);
		std::vector<std::wstring> splitWString(const std::wstring& p_wstr, const std::wstring& p_delimiter);
	}
}