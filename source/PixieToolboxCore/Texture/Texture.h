#pragma once
#include <vector>
#include <glm/glm.hpp>

template<typename T>
class Texture {
public:
	Texture() = default;
	
	Texture(glm::ivec2 resolution) :
		m_resolution(resolution), m_pixels(resolution.x * resolution.y) {}

	Texture(const T* data, glm::ivec2 resolution) :
		m_resolution(resolution), m_pixels(resolution.x * resolution.y) {
		std::copy(data, data + resolution.x * resolution.y, m_pixels.data());
	}

	Texture(const std::vector<T>& data, glm::ivec2 resolution) :
		m_resolution(resolution), m_pixels(resolution.x * resolution.y) {
		std::copy(data.data(), data.data() + resolution.x * resolution.y, m_pixels.data());
	}

	void Resize(glm::ivec2 resolution) {
		m_resolution = resolution;
		m_pixels.resize(resolution.x * resolution.y);
	}

	void SetPixel(uint64_t index, T value) {
		m_pixels[index] = value;
	}

	void SetPixel(glm::ivec2 coords, T value) {
		SetPixel(coords.y * m_resolution.x + coords.x, value);
	}

	glm::ivec2 GetResolution() const {
		return m_resolution;
	}

	uint64_t GetWidth() const {
		return m_resolution.x;
	}

	uint64_t GetHeight() const {
		return m_resolution.y;
	}

	T GetPixel(uint64_t index) const {
		return m_pixels[index];
	}

	T GetPixel(glm::ivec2 coords) const {
		return GetPixel(coords.y * m_resolution.x + coords.x);
	}

	uint64_t GetPixelsCount() const {
		return m_pixels.size();
	}

	uint64_t GetByteSize() const {
		return m_pixels.size() * sizeof(T);
	}

	T* GetData() {
		return m_pixels.data();
	}

protected:
	glm::ivec2 m_resolution = { 0, 0 };
	std::vector<T> m_pixels;
};
