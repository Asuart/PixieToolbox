#pragma once
#include <filesystem>

#include "Texture.h"

class TextureLoader {
public:
	static Texture<float> LoadTextureFloatGreyscale(const std::filesystem::path& filePath);
	static Texture<glm::vec3> LoadTextureFloatRGB(const std::filesystem::path& filePath);
	static Texture<glm::vec4> LoadTextureFloatRGBA(const std::filesystem::path& filePath);
};
