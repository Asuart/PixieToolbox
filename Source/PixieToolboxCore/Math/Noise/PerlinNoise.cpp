#include "PerlinNoise.h"

#include <random>
#include <algorithm>

const std::array<glm::vec2, 4> PerlinNoise::s_constantVectors = {
    glm::vec2(1.0, 1.0),
    glm::vec2(-1.0, 1.0),
    glm::vec2(-1.0, -1.0),
    glm::vec2(1.0, -1.0)
};

PerlinNoise::PerlinNoise(uint32_t seed) {
    SetSeed(seed);
}

void PerlinNoise::SetSeed(uint32_t seed) {
    for (size_t i = 0; i < m_permutations.size(); i++) {
        m_permutations[i] = static_cast<uint8_t>(i);
    }
    std::mt19937 g(seed);
    std::shuffle(m_permutations.begin(), m_permutations.end(), g);
}

float PerlinNoise::Sample(glm::vec2 coords) const {
    uint8_t baseX = static_cast<int32_t>(glm::floor(coords.x)) & 0xff;
    uint8_t baseY = static_cast<int32_t>(glm::floor(coords.y)) & 0xff;

    float offsetX = coords.x - glm::floor(coords.x);
    float offsetY = coords.y - glm::floor(coords.y);

    glm::vec2 topRight = glm::vec2(offsetX - 1.0, offsetY - 1.0);
    glm::vec2 topLeft = glm::vec2(offsetX, offsetY - 1.0);
    glm::vec2 bottomRight = glm::vec2(offsetX - 1.0, offsetY);
    glm::vec2 bottomLeft = glm::vec2(offsetX, offsetY);

    uint8_t indexTopRight = SamplePermuration(SamplePermuration(baseX + 1) + baseY + 1);
    uint8_t indexTopLeft = SamplePermuration(SamplePermuration(baseX) + baseY + 1);
    uint8_t indexBottomRight = SamplePermuration(SamplePermuration(baseX + 1) + baseY);
    uint8_t indexBottomLeft = SamplePermuration(SamplePermuration(baseX) + baseY);

    float dotTopRight = glm::dot(topRight, s_constantVectors[indexTopRight % s_constantVectors.size()]);
    float dotTopLeft = glm::dot(topLeft, s_constantVectors[indexTopLeft % s_constantVectors.size()]);
    float dotBottomRight = glm::dot(bottomRight, s_constantVectors[indexBottomRight % s_constantVectors.size()]);
    float dotBottomLeft = glm::dot(bottomLeft, s_constantVectors[indexBottomLeft % s_constantVectors.size()]);

    float u = SplineWeight(offsetX);
    float v = SplineWeight(offsetY);

    return Lerp(u,
        Lerp(v, dotBottomLeft, dotTopLeft),
        Lerp(v, dotBottomRight, dotTopRight)
    );
}

uint8_t PerlinNoise::SamplePermuration(uint8_t index) const {
    return m_permutations[index];
}

float PerlinNoise::Lerp(float t, float from, float to) const {
    return from + t * (to - from);
}

float PerlinNoise::SplineWeight(float t) const {
    return ((6 * t - 15) * t + 10) * t * t * t;
}