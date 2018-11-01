#include "utils/path_utils.hpp"

#include <utf8.h>

bool utf8_string_to_path(const std::string& str, std::filesystem::path& path)
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		try
		{
			std::wstring wstr;
			utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
			path = wstr;
		}
		catch(...)
		{
			return false;
		}
		return true;
	}

	if constexpr(std::is_same<std::filesystem::path::value_type, char>::value)
	{
		path.clear();
		if(!utf8::is_valid(str.cbegin(), str.cend()))
		{
			return false;
		}
		path = str;
		return true;
	}
}

bool path_to_generic_utf8_string(const std::filesystem::path& path, std::string& str)
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		str.clear();
		std::wstring wstr = path.generic_wstring();
		try
		{
			utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
		}
		catch(...)
		{
			return false;
		}
	}

	if constexpr(std::is_same<std::filesystem::path::value_type, char>::value)
	{
		str = path.generic_string();
		if(!utf8::is_valid(str.cbegin(), str.cend()))
		{
			str.clear();
			return false;
		}
	}

	return true;
}

std::string invalid_utf8_path_representation(const std::filesystem::path& path)
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	std::string str = path_to_generic_utf8_string(path);
	str += "(?) [invalid utf8]";
	return str;
}

std::filesystem::path utf8_string_to_path(const std::string& str)
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		std::wstring wstr;
		try
		{
			utf8::utf8to16(str.begin(), str.end(), std::back_inserter(wstr));
		}
		catch(...)
		{
		}
		return std::filesystem::path(wstr);
	}

	if constexpr(std::is_same<std::filesystem::path::value_type, char>::value)
	{
		return std::filesystem::path(str.cbegin(), utf8::find_invalid(str.cbegin(), str.cend()));
	}
}

std::string path_to_generic_utf8_string(std::filesystem::path path)
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		std::wstring wstr = path.generic_wstring();
		std::string str;
		try
		{
			utf8::utf16to8(wstr.begin(), wstr.end(), std::back_inserter(str));
		}
		catch(...)
		{
		}
		return str;
	}

	if constexpr(std::is_same<std::filesystem::path::value_type, char>::value)
	{
		std::string str = path.generic_string();
		return std::string(str.cbegin(), utf8::find_invalid(str.cbegin(), str.cend()));
	}
}
