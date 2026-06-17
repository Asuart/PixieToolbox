#pragma once
#include "Texture.h"

class TextureUtils {
public:
	template<typename T>
	static Texture<T> ResizeTexture(const Texture<T>& texture, glm::ivec2 newResolution) {
		std::vector<T> resizedData(newResolution.x * newResolution.y);

		int32_t width = texture.GetWidth();
		int32_t height = texture.GetHeight();

		if (newResolution.x < width || newResolution.y < height) {
			throw;
		}

		float rescaleX = static_cast<float>(newResolution.x) / static_cast<float>(width);
		float rescaleY = static_cast<float>(newResolution.y) / static_cast<float>(height);

		for (int32_t y = 0, pixel = 0; y < newResolution.y; y++) {
			for (int32_t x = 0; x < newResolution.x; x++, pixel++) {
				int32_t x0 = glm::min(static_cast<int32_t>(x / rescaleX), width - 1);
				int32_t y0 = glm::min(static_cast<int32_t>(y / rescaleY), height - 1);
				int32_t x1 = (x0 + 1) >= width ? x0 : (x0 + 1);
				int32_t y1 = (y0 + 1) >= height ? y0 : (y0 + 1);

				float u = (x / rescaleX) - static_cast<int32_t>(x / rescaleX);
				float v = (y / rescaleY) - static_cast<int32_t>(y / rescaleY);

				assert(u >= 0.0f && u <= 1.0f);
				assert(v >= 0.0f && v <= 1.0f);

				T v00 = std::log10(texture.GetPixel(y0 * width + x0));
				T v10 = std::log10(texture.GetPixel(y0 * width + x1));
				T v01 = std::log10(texture.GetPixel(y1 * width + x0));
				T v11 = std::log10(texture.GetPixel(y1 * width + x1));

				T bilinearlyInterpolated =
					v00 * (1.0f - u) * (1.0f - v) +
					v10 * u * (1.0f - v) +
					v01 * (1.0f - u) * v +
					v11 * u * v;

				resizedData[pixel] = bilinearlyInterpolated;
			}
		}

		return Texture<T>(resizedData, newResolution);
	}

	template<typename T>
	static Texture<T> SquareBlurFilter(const Texture<T>& texture, glm::ivec2 size) {
		std::vector<T> blurred(texture.GetPixelsCount());
		for (int32_t y = 0, pixel = 0; y < texture.GetHeight(); y++) {
			for (int32_t x = 0; x < texture.GetWidth(); x++, pixel++) {
				float sum = 0.0f;
				float weight = 0.0f;
				for (int32_t r = -size.x; r <= size.x; r++) {
					if (x + r >= 0 && x + r < texture.GetWidth()) {
						sum += texture.GetPixel(pixel + r);
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
	static Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t rangeX, int32_t rangeY) {
		return SquareBlurFilter(texture, { rangeX, rangeY });
	}

	template<typename T>
	static Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t range) {
		return SquareBlurFilter(texture, { range, range });
	}

	template<typename T>
	static Texture<T> GaussianBlurFilter(const Texture<T>& texture, glm::ivec2 range, float scale) {
		const float c1 = 1.0f / (glm::sqrt(TwoPi) * scale);
		const float c2 = -1.0f / (2.0f * scale * scale);

		std::vector<T> blurred(texture.GetPixelsCount());
		for (int32_t y = 0, pixel = 0; y < texture.GetHeight(); y++) {
			for (int32_t x = 0; x < texture.GetWidth(); x++, pixel++) {
				float sum = 0.0f;
				float weight = 0.0f;
				for (int32_t r = -range.x; r <= range.x; r++) {
					if (x + r >= 0 && x + r < texture.GetWidth()) {
						float pixelWeight = c1 * glm::exp(c2 * r * r);
						sum += texture.GetPixel(pixel + r) * pixelWeight;
						weight += pixelWeight;
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
				for (int32_t r = -range.y; r <= range.y; r++) {
					if (y + r >= 0 && y + r < texture.GetHeight()) {
						float pixelWeight = c1 * glm::exp(c2 * r * r);
						sum += blurred[pixel + r * texture.GetWidth()] * pixelWeight;
						weight += pixelWeight;
					}
				}
				blurredTexture.SetPixel(pixel, sum / weight);
			}
		}

		return blurredTexture;
	}

	template<typename T>
	static Texture<T> GaussianlurFilter(const Texture<T>& texture, int32_t rangeX, int32_t rangeY, float scale) {
		return GaussianBlurFilter(texture, { rangeX, rangeY }, scale);
	}

	template<typename T>
	static Texture<T> GaussianBlurFilter(const Texture<T>& texture, int32_t range, float scale) {
		return GaussianBlurFilter(texture, { range, range }, scale);
	}

	template<typename T>
	static Texture<T> MultiplyTexture(const Texture<T>& texture, float scale) {
		Texture<T> scaled = texture;
		for (int32_t pixelIndex = 0; pixelIndex < scaled.GetPixelsCount(); pixelIndex++) {
			scaled.SetPixel(pixelIndex, (scaled.GetPixel(pixelIndex) + 5.0f) * scale);
		}
		return scaled;
	}

	template<typename T>
	static Texture<T> SumTextures(std::vector<Texture<T>> textures) {
		if (textures.size() == 0) {
			return Texture<T>();
		}
		else if (textures.size() == 1) {
			return textures[0];
		}

		for (int32_t i = 1; i < textures.size(); i++) {
			if (textures[i].GetWidth() != textures[0].GetWidth()) {
				throw;
			}
			if (textures[i].GetHeight() != textures[0].GetHeight()) {
				throw;
			}
		}

		Texture<T> result(textures[0].GetResolution());
		for (uint64_t i = 0; i < textures[0].GetPixelsCount(); i++) {
			for (const Texture<T>& texture : textures) {
				result.AccumulatePixel(i, texture.GetPixel(i));
			}
		}

		return result;
	}

	static Texture<float> NormalizeTexture(const Texture<uint8_t>& texture, float newMin = 0.0f, float newMax = 1.0f) {
		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();
		for (uint64_t i = 0; i < texture.GetPixelsCount(); i++) {
			min = glm::min(min, static_cast<float>(texture.GetPixel(i)));
			max = glm::max(max, static_cast<float>(texture.GetPixel(i)));
		}

		Texture<float> result(texture.GetResolution());
		for (uint64_t i = 0; i < texture.GetPixelsCount(); i++) {
			float normalized = (static_cast<float>(texture.GetPixel(i)) + min) / (max - min);
			float remapped = normalized * (newMax - newMin) + newMin;
			result.SetPixel(i, remapped);
		}

		return result;
	}

	static Texture<float> NormalizeTexture(const Texture<float>& texture, float newMin = 0.0f, float newMax = 1.0f) {
		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();
		for (uint64_t i = 0; i < texture.GetPixelsCount(); i++) {
			min = glm::min(min, texture.GetPixel(i));
			max = glm::max(max, texture.GetPixel(i));
		}

		Texture<float> result = texture;
		for (uint64_t i = 0; i < texture.GetPixelsCount(); i++) {
			float normalized = (texture.GetPixel(i) + min) / (max - min);
			float remapped = normalized * (newMax - newMin) + newMin;
			result.SetPixel(i, remapped);
		}

		return result;
	}
};
