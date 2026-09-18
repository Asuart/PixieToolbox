#pragma once
#include <memory>
#include <filesystem>

#include "Scene.h"

namespace PixieToolbox {

class SceneLoader {
  public:
	static std::unique_ptr<Scene> LoadScene(std::filesystem::path path);
  private:
};

} // namespace PixieToolbox
