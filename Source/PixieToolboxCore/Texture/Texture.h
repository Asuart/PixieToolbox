#pragma once
#include <cstdint>
#include <cstring>
#include <glm/glm.hpp>
#include <type_traits>
#include <vector>

#include <PixieRenderer/Image/Image2D.h>
#include <PixieRenderer/Image/ImageUtils.h>

using PixieRenderer::FormatToByteSize;
using PixieRenderer::Image2D;
using PixieRenderer::TextureFormat;

namespace PixieToolbox {

template <typename T> struct TextureFormatOf;

template <> struct TextureFormatOf<uint8_t> {
	static constexpr TextureFormat value = TextureFormat::Red8;
};
template <> struct TextureFormatOf<uint32_t> {
	static constexpr TextureFormat value = TextureFormat::RGBA8;
};
template <> struct TextureFormatOf<float> {
	static constexpr TextureFormat value = TextureFormat::Red32f;
};
template <> struct TextureFormatOf<glm::vec4> {
	static constexpr TextureFormat value = TextureFormat::RGBA32f;
};

template <typename T> class Texture {
  public:
	using value_type = T;

	Texture() = default;

	explicit Texture(glm::uvec2 resolution) {
		m_image.format = TextureFormatOf<T>::value;
		m_image.Resize(resolution);
	}

	Texture(glm::uvec2 resolution, TextureFormat format) {
		m_image.format = format;
		m_image.Resize(resolution);
		ValidateFormat();
	}

	Texture(const T* data, glm::uvec2 resolution) : Texture(resolution) {
		std::memcpy(GetData(), data, m_image.GetByteSize());
	}

	Texture(const std::vector<T>& data, glm::uvec2 resolution) : Texture(resolution) {
		std::memcpy(GetData(), data.data(), m_image.GetByteSize());
	}

	explicit Texture(Image2D& image) : m_image(image) {
		ValidateFormat();
	}

	void Resize(glm::uvec2 resolution) {
		m_image.Resize(resolution);
	}
	void Clear() {
		m_image.Clear();
	}

	T* GetData() {
		return reinterpret_cast<T*>(m_image.pixels.data());
	}
	const T* GetData() const {
		return reinterpret_cast<const T*>(m_image.pixels.data());
	}

	T& GetPixel(uint64_t index) {
		return GetData()[index];
	}

	const T& GetPixel(uint64_t index) const {
		return GetData()[index];
	}

	T& GetPixel(glm::uvec2 coords) {
		return GetPixel(Index(coords));
	}

	const T& GetPixel(glm::uvec2 coords) const {
		return GetPixel(Index(coords));
	}

	void SetPixel(uint64_t index, const T& value) {
		GetData()[index] = value;
	}

	void SetPixel(glm::uvec2 coords, const T& value) {
		SetPixel(Index(coords), value);
	}

	void AccumulatePixel(uint64_t index, const T& value) {
		GetData()[index] += value;
	}
	void AccumulatePixel(glm::uvec2 coords, const T& value) {
		AccumulatePixel(Index(coords), value);
	}

	glm::uvec2 GetResolution() const {
		return m_image.resolution;
	}

	uint32_t GetWidth() const {
		return m_image.resolution.x;
	}

	uint32_t GetHeight() const {
		return m_image.resolution.y;
	}

	uint64_t GetPixelsCount() const {
		return m_image.GetPixelsCount();
	}

	uint64_t GetByteSize() const {
		return m_image.GetByteSize();
	}

	Image2D& GetImage() {
		return m_image;
	}

	const Image2D& GetImage() const {
		return m_image;
	}

  private:
	uint64_t Index(glm::uvec2 coords) const {
		return static_cast<uint64_t>(coords.y) * m_image.resolution.x + coords.x;
	}

	void ValidateFormat() const {
		assert(FormatToByteSize(m_image.format) == sizeof(T));
	}

  private:
	Image2D m_image;
};

} // namespace PixieToolbox
