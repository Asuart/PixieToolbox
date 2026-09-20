#pragma once
#include <filesystem>
#include <memory>

#include <PixieRenderer/Renderer/IRenderer.h>

#include "PixieToolboxCore/Scene/Scene.h"

namespace PixieToolbox {

class SceneLoader {
  public:
	static std::unique_ptr<Scene> LoadScene(std::filesystem::path path, PixieRenderer::IRenderer* renderer);
};

} // namespace PixieToolbox
