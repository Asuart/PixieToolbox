#include "StringUtils.h"

#include <sstream>

std::string StringUtils::TrimLeft(const std::string& str) {
    std::string s = str;
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
        }));
    return s;
}

std::string StringUtils::TrimRight(const std::string& str) {
    std::string s = str;
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
        }).base(), s.end());
    return s;
}

std::string StringUtils::Trim(const std::string& str) {
    return TrimLeft(TrimRight(str));
}

std::vector<std::string> StringUtils::Split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim)) {
        result.push_back(item);
    }

    return result;
}

std::string StringUtils::ReplaceAll(const std::string& str, const std::string& find, const std::string& replace) {
    std::string result;
    size_t find_len = find.size();
    size_t pos, from = 0;
    while (std::string::npos != (pos = str.find(find, from))) {
        result.append(str, from, pos - from);
        result.append(replace);
        from = pos + find_len;
    }
    result.append(str, from, std::string::npos);
    return result;
}

float StringUtils::ToFloat(const std::string& str) {
    try {
        return std::stof(str);
    }
    catch (std::exception e) {
        return 0.0f;
    }
}
