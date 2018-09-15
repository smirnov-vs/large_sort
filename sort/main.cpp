#include <iostream>
#include <fstream>
#include <memory>
#include <vector>
#include <list>
#include <algorithm>

#include <getopt.h>
#include <helpers.h>

void usage()
{
	std::cerr << "Usage: sort -o <output_file> [-i <input_file> -d <working_directory> -b <batch_size> -f <max_temp_files>]" << std::endl;
}

struct InputFile
{
	std::string fileName;
	std::ifstream fs;
	std::string line;
	bool remove;

	InputFile(const std::string& fileName, bool remove = true)
			: fileName(fileName), fs(fileName), remove(remove)
	{
		if (fs)
			std::getline(fs, line);
	}

	~InputFile()
	{
		if (remove)
			::remove(fileName.c_str());
	}
};

bool merge(const std::string& sortedFileName, const std::string& tmpPrefix, size_t tmpFilesCount)
{
	std::ofstream outputFile(tmpPrefix);
	if (!outputFile)
	{
		std::cerr << "aborting: can't create output merge file " << tmpPrefix << std::endl;
		return false;
	}

	std::list<InputFile> files;
	files.emplace_back(sortedFileName, false);
	if (!files.back().fs)
		files.pop_back();
	for (size_t i = 0; i < tmpFilesCount; ++i)
	{
		auto filename = tmpPrefix + '.' + std::to_string(i);
		files.emplace_back(filename);
		if (!files.back().fs)
		{
			std::cerr << "aborting: can't open tmp file " << filename << std::endl;
			return false;
		}
	}

	while (!files.empty())
	{
		auto it = std::min_element(files.begin(), files.end(),
								   [](const InputFile& a, const InputFile& b) { return a.line < b.line; });
		auto& file = *it;
		outputFile << file.line << '\n';
		if (!std::getline(file.fs, file.line))
			files.erase(it);
	}

	if (rename(tmpPrefix.c_str(), sortedFileName.c_str()) != 0)
	{
		std::cerr << "aborting: can't rename merged file " << tmpPrefix << std::endl;
		return false;
	}

	return true;
}

int main(int argc, char* argv[])
{
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr);

	std::string inputFile, outputFile;
	std::string workingDirectory = "/var/tmp";
	size_t batchSize = 1000, maxTmpFiles = 30;

	int c;
	while ((c = getopt(argc, argv, "i:o:d:b:f:")) != -1)
	{
		switch (c)
		{
		case 'i':
			inputFile = optarg;
			break;
		case 'o':
			outputFile = optarg;
			break;
		case 'd':
			workingDirectory = optarg;
			break;
		case 'b':
			batchSize = parseSize(optarg);
			break;
		case 'f':
			maxTmpFiles = parseSize(optarg);
			break;
		default:
			break;
		}
	}

	if (outputFile.empty() || batchSize == 0 || maxTmpFiles == 0)
	{
		usage();
		return 1;
	}

	std::vector<std::string> batch;
	batch.reserve(batchSize);

	std::ifstream fs;
	std::istream& input = [&] () -> std::istream& {
		if (!inputFile.empty())
		{
			fs.open(inputFile);
			return fs;
		}

		return std::cin;
	}();

	auto sortedFileName = workingDirectory + "/sorted";
	auto tmpPrefix = workingDirectory + "/sorted.tmp";
	remove(sortedFileName.c_str());

	size_t numberOfFile = 0;
	std::string line;
	while (input)
	{
		for (size_t i = 0; i < batchSize && std::getline(input, line); ++i)
			batch.emplace_back(std::move(line));

		if (batch.empty())
			continue;

		std::sort(batch.begin(), batch.end());

		auto tmpFileName = tmpPrefix + '.' + std::to_string(numberOfFile);
		std::ofstream tmpFile(tmpFileName);
		if (!tmpFile)
		{
			std::cerr << "aborting: can't create temporary file " << tmpFileName << std::endl;
			return 1;
		}

		for (const auto& i : batch)
			tmpFile << i << '\n';

		batch.clear();
		tmpFile.close();

		++numberOfFile;
		if (numberOfFile == maxTmpFiles)
		{
			if (!merge(sortedFileName, tmpPrefix, maxTmpFiles))
				return 1;
			numberOfFile = 0;
		}
	}

	if (!merge(sortedFileName, tmpPrefix, numberOfFile))
		return 1;

	if (rename(sortedFileName.c_str(), outputFile.c_str()) != 0)
	{
		std::cerr << "aborting: can't rename sorted file " << sortedFileName << std::endl;
		return 1;
	}

	return 0;
}
