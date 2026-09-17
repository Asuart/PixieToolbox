#pragma once
#include <filesystem>

namespace PixieToolbox {

class FileReader {
  public:
	static std::string ReadFileAsString(const std::filesystem::path& filePath);
};

} // namespace PixieToolbox
