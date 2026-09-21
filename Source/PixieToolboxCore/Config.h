#pragma once
#include <filesystem>

namespace PixieToolbox {

class Config {
  public:

	static bool Save(const std::filesystem::path& file = "config.ini");
	static bool Load(const std::filesystem::path& file = "config.ini");

	static void SetApplicationPath(const std::filesystem::path& path);
	static std::filesystem::path GetApplicationPath();
	static std::filesystem::path GetApplicationDirectory();
	static std::filesystem::path ExpandPathToResources(const std::filesystem::path& path);

	static std::filesystem::path GetLastScenePath();
	static void SetLastScenePath(const std::filesystem::path& path);

	static uint32_t GetMaxTextureResolution();
	static void SetMaxTextureResolution(uint32_t size);

  protected:
	static constexpr uint32_t cDefaultMaxTextureResolution = 1024;

	static inline std::filesystem::path s_applicationPath = "";
	static inline std::filesystem::path s_lastScenePath = "";
	static inline uint32_t s_maxTextureResolution = cDefaultMaxTextureResolution;
};

} // namespace PixieToolbox
