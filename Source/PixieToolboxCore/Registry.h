#pragma once
#include <string>
#include <unordered_map>

namespace PixieToolbox {

class Registry {
  public:
	static void SetValue(const std::string& name, const std::string& value);
	static void RemoveValue(const std::string& name);
	static const std::string& GetValue(const std::string& name);

	static void Clear();

	static const std::unordered_map<std::string, std::string>& GetEntries();

  private:
	static std::unordered_map<std::string, std::string> s_entries;
};

} // namespace PixieToolbox
