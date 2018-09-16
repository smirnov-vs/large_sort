#include <helpers.h>
#include <cstdlib>
#include <cerrno>
#include <algorithm>

size_t parseSize(const char* str)
{
	auto num = strtoll(str, nullptr, 10);
	return (size_t)std::max(0LL, num);
}
