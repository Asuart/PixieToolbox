#include "TextureLoader.h"

#include <cstring>
#include <iostream>

#include "PixieToolboxCore/Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace PixieToolbox {

namespace {

std::filesystem::path ResolvePath(const std::filesystem::path& filePath, bool& ok) {
	std::error_code ec;
	if (std::filesystem::exists(filePath, ec) && !ec) {
		ok = true;
		return filePath;
	}

	std::filesystem::path failsafe = Config::ExpandPathToResources("/textures/" + filePath.string());

	if (std::filesystem::exists(failsafe, ec) && !ec) {
		std::cout << "[TextureLoader] Using failsafe path: " << failsafe.string() << '\n';
		ok = true;
		return failsafe;
	}

	ok = false;
	return filePath;
}

} // namespace

Texture<uint8_t> TextureLoader::LoadTextureUInt8Red(const std::filesystem::path& filePath) {
	std::cout << "[TextureLoader] Loading texture: " << filePath.string() << '\n';

	bool ok = false;
	const std::filesystem::path path = ResolvePath(filePath, ok);
	if (!ok) {
		std::cerr << "[TextureLoader] Failed to load texture: " << filePath.string() << '\n';
		return Texture<uint8_t>(glm::uvec2(0, 0));
	}

	int32_t width = 0, height = 0, channels = 0;
	uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 1);
	if (!data) {
		std::cerr << "[TextureLoader] stbi_load failed: " << path.string() << '\n';
		return Texture<uint8_t>(glm::uvec2(0, 0));
	}

	Texture<uint8_t> buffer(glm::uvec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
	std::memcpy(buffer.GetData(), data, buffer.GetByteSize());
	stbi_image_free(data);
	return buffer;
}

Texture<uint32_t> TextureLoader::LoadTextureUInt8RGBA(const std::filesystem::path& filePath) {
	std::cout << "[TextureLoader] Loading texture: " << filePath.string() << '\n';

	bool ok = false;
	const std::filesystem::path path = ResolvePath(filePath, ok);
	if (!ok) {
		std::cerr << "[TextureLoader] Failed to load texture: " << filePath.string() << '\n';
		return Texture<uint32_t>(glm::uvec2(0, 0));
	}

	int32_t width = 0, height = 0, channels = 0;
	uint8_t* data = stbi_load(path.string().c_str(), &width, &height, &channels, 4);
	if (!data) {
		std::cerr << "[TextureLoader] stbi_load failed: " << path.string() << '\n';
		return Texture<uint32_t>(glm::uvec2(0, 0));
	}

	Texture<uint32_t> buffer(glm::uvec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));

	std::memcpy(buffer.GetData(), data, buffer.GetByteSize());
	stbi_image_free(data);
	return buffer;
}

Texture<float> TextureLoader::LoadTextureFloatRed(const std::filesystem::path& filePath) {
	std::cout << "[TextureLoader] Loading texture: " << filePath.string() << '\n';

	bool ok = false;
	const std::filesystem::path path = ResolvePath(filePath, ok);
	if (!ok) {
		std::cerr << "[TextureLoader] Failed to load texture: " << filePath.string() << '\n';
		return Texture<float>(glm::uvec2(0, 0));
	}

	int32_t width = 0, height = 0, channels = 0;
	float* data = stbi_loadf(path.string().c_str(), &width, &height, &channels, 1);
	if (!data) {
		std::cerr << "[TextureLoader] stbi_loadf failed: " << path.string() << '\n';
		return Texture<float>(glm::uvec2(0, 0));
	}

	Texture<float> buffer(glm::uvec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
	std::memcpy(buffer.GetData(), data, buffer.GetByteSize());
	stbi_image_free(data);
	return buffer;
}

Texture<glm::vec4> TextureLoader::LoadTextureFloatRGBA(const std::filesystem::path& filePath) {
	std::cout << "[TextureLoader] Loading texture: " << filePath.string() << '\n';

	bool ok = false;
	const std::filesystem::path path = ResolvePath(filePath, ok);
	if (!ok) {
		std::cerr << "[TextureLoader] Failed to load texture: " << filePath.string() << '\n';
		return Texture<glm::vec4>(glm::uvec2(0, 0));
	}

	int32_t width = 0, height = 0, channels = 0;
	float* data = stbi_loadf(path.string().c_str(), &width, &height, &channels, 4);
	if (!data) {
		std::cerr << "[TextureLoader] stbi_loadf failed: " << path.string() << '\n';
		return Texture<glm::vec4>(glm::uvec2(0, 0));
	}

	Texture<glm::vec4> buffer(glm::uvec2(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
	std::memcpy(buffer.GetData(), data, buffer.GetByteSize());
	stbi_image_free(data);
	return buffer;
}

} // namespace PixieToolbox
