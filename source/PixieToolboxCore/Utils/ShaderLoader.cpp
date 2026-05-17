#include "ShaderLoader.h"
#include "Config.h"
#include "Utils/FileReader.h"
#include "Rendering/RenderEngine.h"

std::string ShaderLoader::LoadShaderSource(const std::string& path) {
	return FileReader::ReadFileAsString(Config::ExpandPathToResources("/shaders/" + path));
}

ShaderHandle ShaderLoader::LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	return RenderEngine::CreateShader(
		LoadShaderSource(vertexShaderPath).c_str(),
		LoadShaderSource(fragmentShaderPath).c_str()
	);
}

ComputeShaderHandle ShaderLoader::LoadComputeShader(const std::string& path) {
	return RenderEngine::CreateComputeShader(
		LoadShaderSource(path).c_str()
	);
}
