#include <core/string.h>

#include <cstdio>
#include <cstdarg>
#include <cassert>

namespace core {

// From somewhere inside chrome...
char* StringWriteInto(std::string& str, std::size_t length_with_null)
{
	if (length_with_null <= 1)
	{
		assert(!"StringWriteInto(): minimum string length with null should be 2");
		return nullptr;
	}

	str.reserve(length_with_null);
	str.resize(length_with_null - 1);
	return &str[0];
}

std::string Sprintf(const char* format, ...)
{
	std::string str;

	va_list args;
	va_start(args, format);

#if defined(_MSVC)
	int length = _vsnprintf(nullptr, 0, format, args);
#else
	int length = 1 * 1024;
#endif

	if (length <= 0)
	{
		return str;
	}

	length = vsnprintf(
		StringWriteInto(str, length + 1),
		length + 1,
		format,
		args);

	va_end(args);


	str.resize(length);
	return str;
}

} // namespace core
