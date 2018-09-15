#include <iostream>
#include <random>
#include <array>
#include <algorithm>
#include <chrono>

#include <getopt.h>
#include <helpers.h>

constexpr auto generateCharset()
{
	constexpr size_t charsetSize = ('9' - '0') + ('z' - 'a') + ('Z' - 'A') + 3;
	std::array<char, charsetSize> chars = { 0 };
	size_t idx = 0;
	for (char c = '0'; c <= '9'; ++c, ++idx)
		chars[idx] = c;
	for (char c = 'a'; c <= 'z'; ++c, ++idx)
		chars[idx] = c;
	for (char c = 'A'; c <= 'Z'; ++c, ++idx)
		chars[idx] = c;
	return chars;
}

void usage()
{
	std::cerr << "Usage: generator -n <number of strings> -m <max line length>" << std::endl;
}

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	size_t numberOfStrings = 0, maxChars = 0;

	int c;
	while ((c = getopt(argc, argv, "n:m:")) != -1)
	{
		switch (c)
		{
		case 'n':
			numberOfStrings = parseSize(optarg);
			break;
		case 'm':
			maxChars = parseSize(optarg);
			break;
		default:
			break;
		}
	}

	if (numberOfStrings == 0 || maxChars == 0)
	{
		usage();
		return 1;
	}

	constexpr auto charset = generateCharset();
	const auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
	std::default_random_engine r(seed);

	std::string line;
	line.reserve(maxChars);
	for (size_t i = 0; i < numberOfStrings; ++i)
	{
		const size_t lineSize = r() % maxChars + 1;
		line.resize(lineSize);
		std::generate_n(line.begin(), lineSize, [&] { return charset[r() % charset.size()]; });
		std::cout << line << '\n';
	}

	return 0;
}
