#pragma once
#include <string>
#include <vector>

namespace PixieToolbox {

class StringUtils {
  public:
	static std::string TrimLeft(const std::string& str);
	static std::string TrimRight(const std::string& str);
	static std::string Trim(const std::string& str);
	static std::vector<std::string> Split(const std::string& s, char delim);
	static std::string ReplaceAll(const std::string& str, const std::string& find, const std::string& replace);

	static float ToFloat(const std::string& str, float fallback = 0.0f);
	static int32_t ToInt32(const std::string& str, int32_t fallback = 0);
	static uint32_t ToUInt32(const std::string& str, uint32_t fallback = 0);
};

} // namespace PixieToolbox
