#pragma once
#include <cstdio>
#include <string>

class Log {
public:
	template<typename... Args>
	inline static void Message(const std::string& message, Args... args) {
		printf((message + "\n").c_str(), args...);
	}

	template<typename... Args>
	inline static void Warning(const std::string& warning, Args... args) {
		printf((warning + "\n").c_str(), args...);
	}

	template<typename... Args>
	inline static void Error(const std::string& error, Args... args) {
		printf((error + "\n").c_str(), args...);
	}
};

class DebugLog {
public:
	template<typename... Args>
	static void Message(const std::string& message, Args... args) {
#ifndef NDEBUG
		printf((message + "\n").c_str(), args...);
#endif
	}

	template<typename... Args>
	static void Warning(const std::string& warning, Args... args) {
#ifndef NDEBUG
		printf((warning + "\n").c_str(), args...);
#endif
	}

	template<typename... Args>
	static void Error(const std::string& error, Args... args) {
#ifndef NDEBUG
		printf((error + "\n").c_str(), args...);
#endif
	}
};
