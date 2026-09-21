#include "Config.h"

#include <fstream>
#include <string>

#include "Utils/StringUtils.h"

namespace PixieToolbox {

static const std::string cLastSceneKeyName = "lastScene";
static const std::string cMaxTextureResolutionKeyName = "maxTextureResolution";

bool Config::Load(const std::filesystem::path& file) {
	std::ifstream in(file);
	if (!in) {
		return false;
	}

	std::string line;
	while (std::getline(in, line)) {
		line = StringUtils::Trim(line);
		if (line.empty() || line[0] == '#' || line[0] == ';') {
			continue;
		}

		const auto eq = line.find('=');
		if (eq == std::string::npos) {
			continue;
		}

		const std::string key = StringUtils::Trim(line.substr(0, eq));
		const std::string val = StringUtils::Trim(line.substr(eq + 1));

		if (key == cLastSceneKeyName) {
			s_lastScenePath = val;
		} else if (key == cMaxTextureResolutionKeyName) {
			uint32_t resolution = StringUtils::ToUInt32(val, cDefaultMaxTextureResolution);
			if (resolution == 0) {
				resolution = 1;
			}
			s_maxTextureResolution = resolution;
		}
	}
	return true;
}

bool Config::Save(const std::filesystem::path& file) {
	std::ofstream out(file, std::ios::trunc);
	if (!out) {
		return false;
	}

	out << cLastSceneKeyName << "=" << s_lastScenePath.string() << "\n";
	out << cMaxTextureResolutionKeyName << "=" << s_maxTextureResolution << "\n";

	return true;
}

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

std::filesystem::path Config::GetLastScenePath() {
	return s_lastScenePath;
}

void Config::SetLastScenePath(const std::filesystem::path& path) {
	s_lastScenePath = path;
	Save();
}

uint32_t Config::GetMaxTextureResolution() {
	return s_maxTextureResolution;
}

void Config::SetMaxTextureResolution(uint32_t size) {
	if (size == 0) {
		size = 1;
	}
	s_maxTextureResolution = size;
	Save();
}

} // namespace PixieToolbox
