#include "TextureLoader.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <iostream>
#include <type_traits>
#include <vector>

#include "PixieToolboxCore/Config.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

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

bool ComputeResizedSize(int w, int h, int& nw, int& nh) {
	if (w <= Config::s_maxTextureResolution && h <= Config::s_maxTextureResolution) {
		return false;
	}
	const float scale = std::
	    min(static_cast<float>(Config::s_maxTextureResolution) / static_cast<float>(w),
	        static_cast<float>(Config::s_maxTextureResolution) / static_cast<float>(h));
	nw = std::max(1, static_cast<int>(std::lround(static_cast<float>(w) * scale)));
	nh = std::max(1, static_cast<int>(std::lround(static_cast<float>(h) * scale)));
	return true;
}

template <typename T>
std::vector<T> LoadResizedPixels(
    const std::filesystem::path& filePath,
    int desiredChannels,
    stbir_pixel_layout layout,
    uint32_t& outW,
    uint32_t& outH
) {
	std::cout << "[TextureLoader] Loading texture: " << filePath.string() << '\n';

	bool ok = false;
	const std::filesystem::path path = ResolvePath(filePath, ok);
	if (!ok) {
		std::cerr << "[TextureLoader] Failed to find texture: " << filePath.string() << '\n';
		return {};
	}

	int width = 0, height = 0, channels = 0;
	T* data = nullptr;
	if constexpr (std::is_same_v<T, float>) {
		data = stbi_loadf(path.string().c_str(), &width, &height, &channels, desiredChannels);
	} else {
		data = reinterpret_cast<T*>(stbi_load(path.string().c_str(), &width, &height, &channels, desiredChannels));
	}
	if (!data) {
		std::cerr << "[TextureLoader] Failed to decode: " << path.string() << '\n';
		return {};
	}

	int nw = 0, nh = 0;
	if (ComputeResizedSize(width, height, nw, nh)) {
		std::cout << "[TextureLoader] Resizing " << width << 'x' << height << " -> " << nw << 'x' << nh << ": "
		          << path.string() << '\n';

		std::vector<T> resized(
		    static_cast<size_t>(nw) * static_cast<size_t>(nh) * static_cast<size_t>(desiredChannels)
		);

		T* result = nullptr;
		if constexpr (std::is_same_v<T, float>) {
			result = stbir_resize_float_linear(data, width, height, 0, resized.data(), nw, nh, 0, layout);
		} else {
			result = reinterpret_cast<T*>(stbir_resize_uint8_linear(
			    reinterpret_cast<const unsigned char*>(data),
			    width,
			    height,
			    0,
			    reinterpret_cast<unsigned char*>(resized.data()),
			    nw,
			    nh,
			    0,
			    layout
			));
		}
		stbi_image_free(data);

		if (!result) {
			std::cerr << "[TextureLoader] Resize failed: " << path.string() << '\n';
			return {};
		}

		outW = static_cast<uint32_t>(nw);
		outH = static_cast<uint32_t>(nh);
		return resized;
	}

	std::vector<T> pixels(
	    static_cast<size_t>(width) * static_cast<size_t>(height) * static_cast<size_t>(desiredChannels)
	);
	std::memcpy(pixels.data(), data, pixels.size() * sizeof(T));
	stbi_image_free(data);

	outW = static_cast<uint32_t>(width);
	outH = static_cast<uint32_t>(height);
	return pixels;
}

} // namespace

Texture<uint8_t> TextureLoader::LoadTextureUInt8Red(const std::filesystem::path& filePath) {
	uint32_t w = 0, h = 0;
	auto pixels = LoadResizedPixels<uint8_t>(filePath, 1, STBIR_1CHANNEL, w, h);
	if (pixels.empty()) {
		return Texture<uint8_t>(glm::uvec2(0, 0));
	}
	Texture<uint8_t> buffer(glm::uvec2(w, h));
	std::memcpy(buffer.GetData(), pixels.data(), buffer.GetByteSize());
	return buffer;
}

Texture<uint32_t> TextureLoader::LoadTextureUInt8RGBA(const std::filesystem::path& filePath) {
	uint32_t w = 0, h = 0;
	auto pixels = LoadResizedPixels<uint8_t>(filePath, 4, STBIR_RGBA, w, h);
	if (pixels.empty()) {
		return Texture<uint32_t>(glm::uvec2(0, 0));
	}
	Texture<uint32_t> buffer(glm::uvec2(w, h));
	std::memcpy(buffer.GetData(), pixels.data(), buffer.GetByteSize());
	return buffer;
}

Texture<float> TextureLoader::LoadTextureFloatRed(const std::filesystem::path& filePath) {
	uint32_t w = 0, h = 0;
	auto pixels = LoadResizedPixels<float>(filePath, 1, STBIR_1CHANNEL, w, h);
	if (pixels.empty()) {
		return Texture<float>(glm::uvec2(0, 0));
	}
	Texture<float> buffer(glm::uvec2(w, h));
	std::memcpy(buffer.GetData(), pixels.data(), buffer.GetByteSize());
	return buffer;
}

Texture<glm::vec4> TextureLoader::LoadTextureFloatRGBA(const std::filesystem::path& filePath) {
	uint32_t w = 0, h = 0;
	auto pixels = LoadResizedPixels<float>(filePath, 4, STBIR_RGBA, w, h);
	if (pixels.empty()) {
		return Texture<glm::vec4>(glm::uvec2(0, 0));
	}
	Texture<glm::vec4> buffer(glm::uvec2(w, h));
	std::memcpy(buffer.GetData(), pixels.data(), buffer.GetByteSize());
	return buffer;
}

} // namespace PixieToolbox
