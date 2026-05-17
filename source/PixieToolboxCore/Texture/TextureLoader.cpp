#include "TextureLoader.h"

#include "Config.h"
#include "Log/Log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture<float> TextureLoader::LoadTextureFloatGreyscale(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 1);
	if (!data) {
		std::filesystem::path failsafePath = Config::ExpandPathToResources("/textures/" + filePath.string());
		data = stbi_loadf(failsafePath.string().c_str(), &width, &height, &nrChannels, 1);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture<float>({ 0, 0 });
		}
		else {
			Log::Warning("Loaded using failsafe path: %s", failsafePath.string().c_str());
		}
	}
	Texture<float> buffer({ width, height });
	std::copy(data, data + buffer.GetByteSize(), buffer.GetData());
	stbi_image_free(data);
	return buffer;
}

Texture<glm::vec3> TextureLoader::LoadTextureFloatRGB(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 3);
	if (!data) {
		std::filesystem::path failsafePath = Config::ExpandPathToResources("/textures/" + filePath.string());
		data = stbi_loadf(failsafePath.string().c_str(), &width, &height, &nrChannels, 3);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture<glm::vec3>({ 0, 0 });
		}
		else {
			Log::Warning("Loaded using failsafe path: %s", failsafePath.string().c_str());
		}
	}
	Texture<glm::vec3> buffer({ width, height });
	std::copy(data, data + buffer.GetByteSize(), buffer.GetData());
	stbi_image_free(data);
	return buffer;
}

Texture<glm::vec4> TextureLoader::LoadTextureFloatRGBA(const std::filesystem::path& filePath) {
	Log::Message("Loading texture: %s", filePath.string().c_str());
	int32_t width, height, nrChannels;
	float* data = stbi_loadf(filePath.string().c_str(), &width, &height, &nrChannels, 4);
	if (!data) {
		std::filesystem::path failsafePath = Config::ExpandPathToResources("/textures/" + filePath.string());
		data = stbi_loadf(failsafePath.string().c_str(), &width, &height, &nrChannels, 4);
		if (!data) {
			Log::Error("Failed to load texture");
			return Texture<glm::vec4>({ 0 , 0 });
		}
		else {
			Log::Warning("Loaded using failsafe path: %s", failsafePath.string().c_str());
		}
	}
	Texture<glm::vec4> buffer({ width, height });
	std::copy(data, data + buffer.GetByteSize(), buffer.GetData());
	stbi_image_free(data);
	return buffer;
}
