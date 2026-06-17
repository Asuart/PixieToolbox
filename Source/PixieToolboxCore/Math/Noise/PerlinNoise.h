#pragma once
#include <array>
#include <glm/glm.hpp>

class PerlinNoise {
public:
	PerlinNoise(uint32_t seed = 0);

    void SetSeed(uint32_t seed);

	float Sample(glm::vec2 coords) const;

private:
	static const std::array<glm::vec2, 4> s_constantVectors;
	std::array<uint8_t, 256> m_permutations = { 0 };

	uint8_t SamplePermuration(uint8_t index) const;
	float Lerp(float t, float from, float to) const;
	float SplineWeight(float t) const;
};
