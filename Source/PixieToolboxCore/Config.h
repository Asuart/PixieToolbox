#pragma once
#include <filesystem>

namespace PixieToolbox {

class Config {
  public:
	static void SetApplicationPath(const std::filesystem::path& path);

	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path ExpandPathToResources(const std::filesystem::path& path);

	inline static uint32_t s_maxTextureResolution = 1024;

  protected:
	static std::filesystem::path s_applicationPath;
};

} // namespace PixieToolbox
