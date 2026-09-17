#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "Texture.h"

namespace PixieToolbox {

template <typename T> struct PixelAccumulator {
	using type = T;
	static type From(const T& v) {
		return v;
	}
	static T To(const type& v) {
		return v;
	}
};

template <> struct PixelAccumulator<uint8_t> {
	using type = float;
	static type From(uint8_t v) {
		return static_cast<float>(v);
	}
	static uint8_t To(const type& v) {
		return static_cast<uint8_t>(std::clamp(v, 0.0f, 255.0f));
	}
};

template <> struct PixelAccumulator<uint16_t> {
	using type = float;
	static type From(uint16_t v) {
		return static_cast<float>(v);
	}
	static uint16_t To(const type& v) {
		return static_cast<uint16_t>(std::clamp(v, 0.0f, 65535.0f));
	}
};

class TextureUtils {
  public:
	template <typename T> static Texture<T> ResizeTexture(const Texture<T>& texture, glm::uvec2 newResolution) {
		using Acc = typename PixelAccumulator<T>::type;

		const uint32_t width = texture.GetWidth();
		const uint32_t height = texture.GetHeight();

		if (newResolution.x == width && newResolution.y == height) {
			return texture;
		}
		if (width == 0 || height == 0) {
			return Texture<T>(newResolution);
		}

		Texture<T> result(newResolution);

		const float rescaleX = static_cast<float>(newResolution.x) / static_cast<float>(width);
		const float rescaleY = static_cast<float>(newResolution.y) / static_cast<float>(height);

		for (uint32_t y = 0; y < newResolution.y; ++y) {
			const float srcY = (static_cast<float>(y) + 0.5f) / rescaleY - 0.5f;
			const int32_t y0 = std::clamp(static_cast<int32_t>(std::floor(srcY)), 0, static_cast<int32_t>(height) - 1);
			const int32_t y1 = std::min(y0 + 1, static_cast<int32_t>(height) - 1);
			const float v = std::clamp(srcY - static_cast<float>(y0), 0.0f, 1.0f);

			for (uint32_t x = 0; x < newResolution.x; ++x) {
				const float srcX = (static_cast<float>(x) + 0.5f) / rescaleX - 0.5f;
				const int32_t
				    x0 = std::clamp(static_cast<int32_t>(std::floor(srcX)), 0, static_cast<int32_t>(width) - 1);
				const int32_t x1 = std::min(x0 + 1, static_cast<int32_t>(width) - 1);
				const float u = std::clamp(srcX - static_cast<float>(x0), 0.0f, 1.0f);

				const Acc v00 = PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y0) * width + x0));
				const Acc v10 = PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y0) * width + x1));
				const Acc v01 = PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y1) * width + x0));
				const Acc v11 = PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y1) * width + x1));

				const Acc top = v00 * (1.0f - u) + v10 * u;
				const Acc bottom = v01 * (1.0f - u) + v11 * u;
				const Acc value = top * (1.0f - v) + bottom * v;

				result.SetPixel(glm::uvec2(x, y), PixelAccumulator<T>::To(value));
			}
		}

		return result;
	}

	template <typename T> static Texture<T> SquareBlurFilter(const Texture<T>& texture, glm::uvec2 range) {
		using Acc = typename PixelAccumulator<T>::type;

		const uint32_t width = texture.GetWidth();
		const uint32_t height = texture.GetHeight();
		if (width == 0 || height == 0)
			return texture;

		const int32_t rx = static_cast<int32_t>(range.x);
		const int32_t ry = static_cast<int32_t>(range.y);

		std::vector<Acc> tmp(static_cast<size_t>(width) * height);
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				Acc sum{};
				float weight = 0.0f;
				const int32_t x0 = std::max(0, static_cast<int32_t>(x) - rx);
				const int32_t x1 = std::min(static_cast<int32_t>(width) - 1, static_cast<int32_t>(x) + rx);
				for (int32_t xi = x0; xi <= x1; ++xi) {
					sum += PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y) * width + xi));
					weight += 1.0f;
				}
				tmp[static_cast<size_t>(y) * width + x] = sum / weight;
			}
		}

		Texture<T> blurred(texture.GetResolution());
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				Acc sum{};
				float weight = 0.0f;
				const int32_t y0 = std::max(0, static_cast<int32_t>(y) - ry);
				const int32_t y1 = std::min(static_cast<int32_t>(height) - 1, static_cast<int32_t>(y) + ry);
				for (int32_t yi = y0; yi <= y1; ++yi) {
					sum += tmp[static_cast<size_t>(yi) * width + x];
					weight += 1.0f;
				}
				blurred.SetPixel(glm::uvec2(x, y), PixelAccumulator<T>::To(sum / weight));
			}
		}

		return blurred;
	}

	template <typename T>
	static Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t rangeX, int32_t rangeY) {
		return SquareBlurFilter(texture, glm::uvec2(rangeX, rangeY));
	}

	template <typename T> static Texture<T> SquareBlurFilter(const Texture<T>& texture, int32_t range) {
		return SquareBlurFilter(texture, glm::uvec2(range, range));
	}

	template <typename T>
	static Texture<T> GaussianBlurFilter(const Texture<T>& texture, glm::uvec2 range, float sigma) {
		using Acc = typename PixelAccumulator<T>::type;

		const uint32_t width = texture.GetWidth();
		const uint32_t height = texture.GetHeight();
		if (width == 0 || height == 0)
			return texture;
		if (sigma <= 0.0f)
			return texture;

		const float c1 = 1.0f / (std::sqrt(glm::two_pi<float>()) * sigma);
		const float c2 = -1.0f / (2.0f * sigma * sigma);

		const int32_t rx = static_cast<int32_t>(range.x);
		const int32_t ry = static_cast<int32_t>(range.y);

		std::vector<Acc> tmp(static_cast<size_t>(width) * height);
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				Acc sum{};
				float weight = 0.0f;
				const int32_t x0 = std::max(0, static_cast<int32_t>(x) - rx);
				const int32_t x1 = std::min(static_cast<int32_t>(width) - 1, static_cast<int32_t>(x) + rx);
				for (int32_t xi = x0; xi <= x1; ++xi) {
					const float dx = static_cast<float>(xi - static_cast<int32_t>(x));
					const float w = c1 * std::exp(c2 * dx * dx);
					sum += PixelAccumulator<T>::From(texture.GetPixel(static_cast<uint64_t>(y) * width + xi)) * w;
					weight += w;
				}
				tmp[static_cast<size_t>(y) * width + x] = sum / weight;
			}
		}

		Texture<T> blurred(texture.GetResolution());
		for (uint32_t y = 0; y < height; ++y) {
			for (uint32_t x = 0; x < width; ++x) {
				Acc sum{};
				float weight = 0.0f;
				const int32_t y0 = std::max(0, static_cast<int32_t>(y) - ry);
				const int32_t y1 = std::min(static_cast<int32_t>(height) - 1, static_cast<int32_t>(y) + ry);
				for (int32_t yi = y0; yi <= y1; ++yi) {
					const float dy = static_cast<float>(yi - static_cast<int32_t>(y));
					const float w = c1 * std::exp(c2 * dy * dy);
					sum += tmp[static_cast<size_t>(yi) * width + x] * w;
					weight += w;
				}
				blurred.SetPixel(glm::uvec2(x, y), PixelAccumulator<T>::To(sum / weight));
			}
		}

		return blurred;
	}

	template <typename T>
	static Texture<T> GaussianBlurFilter(const Texture<T>& texture, int32_t rangeX, int32_t rangeY, float sigma) {
		return GaussianBlurFilter(texture, glm::uvec2(rangeX, rangeY), sigma);
	}

	template <typename T> static Texture<T> GaussianBlurFilter(const Texture<T>& texture, int32_t range, float sigma) {
		return GaussianBlurFilter(texture, glm::uvec2(range, range), sigma);
	}

	template <typename T> static Texture<T> MultiplyTexture(const Texture<T>& texture, float scale) {
		Texture<T> result(texture.GetResolution());
		const uint64_t pixels = texture.GetPixelsCount();
		for (uint64_t i = 0; i < pixels; ++i) {
			const auto v = PixelAccumulator<T>::From(texture.GetPixel(i)) * scale;
			result.SetPixel(i, PixelAccumulator<T>::To(v));
		}
		return result;
	}

	template <typename T> static Texture<T> SumTextures(const std::vector<Texture<T>>& textures) {
		using Acc = typename PixelAccumulator<T>::type;

		if (textures.empty()) {
			return Texture<T>();
		}
		if (textures.size() == 1) {
			return textures.front();
		}

		const glm::uvec2 res = textures.front().GetResolution();
		for (size_t i = 1; i < textures.size(); ++i) {
			if (textures[i].GetResolution() != res) {
				throw std::invalid_argument("SumTextures: texture size mismatch");
			}
		}

		Texture<T> result(res);
		const uint64_t pixels = textures.front().GetPixelsCount();
		for (uint64_t i = 0; i < pixels; ++i) {
			Acc sum{};
			for (const auto& tex : textures) {
				sum += PixelAccumulator<T>::From(tex.GetPixel(i));
			}
			result.SetPixel(i, PixelAccumulator<T>::To(sum));
		}
		return result;
	}

	template <typename T>
	static std::enable_if_t<std::is_arithmetic_v<T>, Texture<float>> NormalizeTexture(
	    const Texture<T>& texture,
	    float newMin = 0.0f,
	    float newMax = 1.0f
	) {
		const uint64_t pixels = texture.GetPixelsCount();
		if (pixels == 0) {
			return Texture<float>(texture.GetResolution());
		}

		float minV = std::numeric_limits<float>::infinity();
		float maxV = -std::numeric_limits<float>::infinity();
		for (uint64_t i = 0; i < pixels; ++i) {
			const float v = static_cast<float>(texture.GetPixel(i));
			minV = std::min(minV, v);
			maxV = std::max(maxV, v);
		}

		const float range = maxV - minV;
		const float invRange = (range > 0.0f) ? (1.0f / range) : 0.0f;

		Texture<float> result(texture.GetResolution());
		for (uint64_t i = 0; i < pixels; ++i) {
			const float v = static_cast<float>(texture.GetPixel(i));
			const float n = (v - minV) * invRange;
			const float r = n * (newMax - newMin) + newMin;
			result.SetPixel(i, r);
		}
		return result;
	}
};

} // namespace PixieToolbox
