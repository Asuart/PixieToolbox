#pragma once
#include <string>

#include "Rendering/ResourceHandles.h"

class ShaderLoader {
public:
	static std::string LoadShaderSource(const std::string& path);
	static ShaderHandle LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath);
	static ComputeShaderHandle LoadComputeShader(const std::string& path);
};
