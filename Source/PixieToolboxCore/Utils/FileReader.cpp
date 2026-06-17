#include "FileReader.h"

#include <iostream>
#include <fstream>

std::string FileReader::ReadFileAsString(const std::filesystem::path& filePath) {
	std::ifstream t(filePath);
	if (!t.is_open()) {
		std::cout << "Failed to open file: \"" << filePath.string() << "\"\n";
		return "";
	}
	std::string file((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	return file;
}
