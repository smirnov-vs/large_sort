#include <helpers.h>
#include <cstdlib>

size_t parseSize(const char* str)
{
	char* end;
	auto num = strtoull(str, &end, 10);
	return num;
}
