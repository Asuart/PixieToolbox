#pragma once
#include <filesystem>

#include "Texture.h"

namespace PixieToolbox {

class TextureLoader {
  public:
	static Texture<uint8_t> LoadTextureUInt8Red(const std::filesystem::path& filePath);
	static Texture<uint32_t> LoadTextureUInt8RGBA(const std::filesystem::path& filePath);
	static Texture<float> LoadTextureFloatRed(const std::filesystem::path& filePath);
	static Texture<glm::vec4> LoadTextureFloatRGBA(const std::filesystem::path& filePath);
};

} // namespace PixieToolbox
