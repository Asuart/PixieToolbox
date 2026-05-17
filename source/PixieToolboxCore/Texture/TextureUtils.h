#pragma once
#include "Texture.h"

class TextureUtils {
public:

	template<typename T>
	Texture<T> ResizeTexture(const Texture<T>& texture, glm::ivec2 newResolution) {

	}

	template<typename T>
	Texture<T> SquareBlurFilter(const Texture<T>& texture, glm::ivec2 size) {
		std::vector<T> blurred(texture.GetPixelsCount());
		for (int32_t y = 0, pixel = 0; y < texture.GetHeight(); y++) {
			for (int32_t x = 0; x < texture.GetWidth(); x++, pixel++) {
				float sum = 0.0f;
				float weight = 0.0f;
				for (int32_t r = -size.x; r <= size.x; r++) {
					if (x + r >= 0 && x + r < texture.GetWidth()) {
						sum += heightMap[pixel + r];
						weight += 1.0f;
					}
				}
				blurred[pixel] = sum / weight;
			}
		}

		Texture<T> blurredTexture(texture.GetResolution());
		for (int32_t y = 0, pixel = 0; y < texture.GetHeight(); y++) {
			for (int32_t x = 0; x < texture.GetWidth(); x++, pixel++) {
				float sum = 0.0f;
				float weight = 0.0f;
				for (int32_t r = -size.y; r <= size.y; r++) {
					if (y + r >= 0 && y + r < texture.GetHeight()) {
						sum += blurred[pixel + r * texture.GetWidth()];
						weight += 1.0f;
					}
				}
				blurredTexture.SetPixel(pixel, sum / weight);
			}
		}

		return blurredTexture;
	}

	template<typename T>
	Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t x, int32_t y) {
		return SquareBlurFilter(texture, { x, y });
	}

	template<typename T>
	Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t size) {
		return SquareBlurFilter(texture, { size, size });
	}
};
