#pragma once
#include <string>
#include <vector>

class StringUtils {
public:
    static std::string TrimLeft(const std::string& str);
    static std::string TrimRight(const std::string& str);
    static std::string Trim(const std::string& str);
    static std::vector<std::string> Split(const std::string& s, char delim);
    static std::string ReplaceAll(const std::string& str, const std::string& find, const std::string& replace);

    static float ToFloat(const std::string& str);
};
