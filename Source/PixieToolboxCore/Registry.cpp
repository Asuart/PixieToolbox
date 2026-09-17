#include "Registry.h"

namespace PixieToolbox {

void Registry::SetValue(const std::string& name, const std::string& value) {
	s_entries[name] = value;
}

void Registry::RemoveValue(const std::string& name) {
	if (s_entries.contains(name)) {
		s_entries.erase(name);
	}
}

const std::string& Registry::GetValue(const std::string& name) {
	if (s_entries.contains(name)) {
		return s_entries[name];
	}
	static const std::string fallback = "Undefined";
	return fallback;
}

void Registry::Clear() {
	s_entries.clear();
}

const std::unordered_map<std::string, std::string>& Registry::GetEntries() {
	return s_entries;
}

} // namespace PixieToolbox
