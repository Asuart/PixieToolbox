#include "ShaderLoader.h"
#include "Config.h"
#include "Utils/FileReader.h"
#include "Rendering/RenderEngine.h"

ShaderHandle ShaderLoader::LoadShader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {
	return RenderEngine::CreateShader(
		FileReader::ReadFileAsString(Config::ExpandPathToResources("/shaders/" + vertexShaderPath)).c_str(),
		FileReader::ReadFileAsString(Config::ExpandPathToResources("/shaders/" + fragmentShaderPath)).c_str()
	);
}

ComputeShaderHandle ShaderLoader::LoadComputeShader(const std::string& path) {
	return RenderEngine::CreateComputeShader(
		FileReader::ReadFileAsString(Config::ExpandPathToResources("/shaders/" + path)).c_str()
	);
}
