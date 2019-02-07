#include "utils/path_utils.hpp"

#include <utf8.h>

utf8_path::utf8_path() noexcept: m_valid(), m_utf8_str(), m_path()
{
}

utf8_path::utf8_path(const utf8_path& other) noexcept
  : m_valid(other.m_valid), m_utf8_str(other.m_utf8_str), m_path(other.m_path)
{
}

utf8_path::utf8_path(utf8_path&& other) noexcept
  : m_valid(other.m_valid), m_utf8_str(std::move(other.m_utf8_str)), m_path(std::move(other.m_path))
{
}

utf8_path::utf8_path(const char* str) noexcept: m_valid(), m_utf8_str(str), m_path()
{
	set_path();
}

utf8_path::utf8_path(std::string_view str) noexcept: m_valid(), m_utf8_str(str), m_path()
{
	set_path();
}

utf8_path::utf8_path(std::string&& str) noexcept: m_valid(), m_utf8_str(std::move(str)), m_path()
{
	set_path();
}

utf8_path::utf8_path(const std::filesystem::path& path) noexcept
  : m_valid(), m_utf8_str(), m_path(path)
{
	set_str();
}

utf8_path::utf8_path(std::filesystem::path&& path) noexcept
  : m_valid(), m_utf8_str(), m_path(std::move(path))
{
	set_str();
}

utf8_path& utf8_path::operator=(const utf8_path& other) noexcept
{
	if(this != &other)
	{
		m_valid = other.m_valid;
		m_utf8_str = other.m_utf8_str;
		m_path = other.m_path;
	}
	return *this;
}

utf8_path& utf8_path::operator=(utf8_path&& other) noexcept
{
	if(this != &other)
	{
		m_valid = other.m_valid;
		m_utf8_str = std::move(other.m_utf8_str);
		m_path = std::move(other.m_path);
	}
	return *this;
}

utf8_path& utf8_path::operator=(const char* str) noexcept
{
	m_utf8_str = str;
	set_path();
	return *this;
}

utf8_path& utf8_path::operator=(std::string_view str) noexcept
{
	m_utf8_str = str;
	set_path();
	return *this;
}

utf8_path& utf8_path::operator=(std::string&& str) noexcept
{
	m_utf8_str = std::move(str);
	set_path();
	return *this;
}

utf8_path& utf8_path::operator=(const std::filesystem::path& path) noexcept
{
	m_path = path;
	set_str();
	return *this;
}

utf8_path& utf8_path::operator=(std::filesystem::path&& path) noexcept
{
	m_path = std::move(path);
	set_str();
	return *this;
}

std::string_view utf8_path::str() const noexcept
{
	return m_utf8_str;
}

const std::string& utf8_path::str_cref() const noexcept
{
	return m_utf8_str;
}

const std::filesystem::path& utf8_path::path() const noexcept
{
	return m_path;
}

void utf8_path::set_str() noexcept
{
	m_valid = path_to_generic_utf8_string(m_path, m_utf8_str);
	if(!m_valid)
	{
		m_utf8_str = path_to_generic_utf8_string(m_path);
	}
}
void utf8_path::set_path() noexcept
{
	m_valid = utf8_string_to_path(m_utf8_str, m_path);
	if(!m_valid)
	{
		m_path = utf8_string_to_path(m_utf8_str);
	}
}

bool utf8_path::valid() const noexcept
{
	return m_valid;
}

std::ostream& operator<<(std::ostream& os, const utf8_path& utf8_path)
{
	return os << '\"' << utf8_path.str() << '\"';
}

bool utf8_string_to_path(const std::string_view str, std::filesystem::path& path) noexcept
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		try
		{
			std::wstring wstr;
			utf8::utf8to16(str.cbegin(), str.cend(), std::back_inserter(wstr));
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

bool path_to_generic_utf8_string(const std::filesystem::path& path, std::string& str) noexcept
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

std::string invalid_utf8_path_representation(const std::filesystem::path& path) noexcept
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	std::string str = path_to_generic_utf8_string(path);
	str += "(?) [invalid utf8]";
	return str;
}

std::filesystem::path utf8_string_to_path(const std::string_view str) noexcept
{
	static_assert(std::is_same<std::filesystem::path::value_type, wchar_t>::value
	                || std::is_same<std::filesystem::path::value_type, char>::value,
	              "std::filesystem::path::value_type has an unsupported type");

	if constexpr(std::is_same<std::filesystem::path::value_type, wchar_t>::value)
	{
		std::wstring wstr;
		try
		{
			utf8::utf8to16(str.cbegin(), str.cend(), std::back_inserter(wstr));
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

std::string path_to_generic_utf8_string(std::filesystem::path path) noexcept
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
