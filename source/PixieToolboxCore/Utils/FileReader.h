#pragma once
#include <filesystem>

class FileReader {
public:
	static std::string ReadFileAsString(const std::filesystem::path& filePath);
};
