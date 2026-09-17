#pragma once
#include "PixieToolboxCore/Math/Noise/PerlinNoise.h"
#include "Texture.h"

namespace PixieToolbox {

class TextureGenerator {
  public:
	static Texture<float> GeneratePerlinNoise(
	    glm::ivec2 resolution,
	    uint32_t seed,
	    glm::vec2 uvScale,
	    float amplitudeScale
	);
};

} // namespace PixieToolbox
