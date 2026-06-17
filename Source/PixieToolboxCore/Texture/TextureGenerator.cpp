#include "TextureGenerator.h"

Texture<float> TextureGenerator::GeneratePerlinNoise(glm::ivec2 resolution, uint32_t seed, glm::vec2 uvScale, float amplitudeScale) {
	PerlinNoise perlinNoise(seed);
	Texture<float> noiseTexture(resolution);
	for (uint64_t y = 0; y < noiseTexture.GetHeight(); y++) {
		for (uint64_t x = 0; x < noiseTexture.GetWidth(); x++) {
			glm::vec2 uv = glm::vec2(x, y) / glm::vec2(resolution.x - 1, resolution.y - 1);
			float value = perlinNoise.Sample(uv * uvScale) * amplitudeScale;
			noiseTexture.SetPixel({ x, y }, value);
		}
	}
	return noiseTexture;
}
