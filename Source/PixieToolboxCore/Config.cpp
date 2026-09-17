#include "Config.h"

namespace PixieToolbox {

std::filesystem::path Config::s_applicationPath = "";

void Config::SetApplicationPath(const std::filesystem::path& path) {
	s_applicationPath = path;
}

std::filesystem::path Config::GetApplicationPath() {
	return s_applicationPath;
}

std::filesystem::path Config::GetApplicationDirectory() {
	return s_applicationPath.parent_path();
}

std::filesystem::path Config::ExpandPathToResources(const std::filesystem::path& path) {
	return GetApplicationDirectory().string() + std::string("/resources/") + path.string();
}

} // namespace PixieToolbox
