#pragma once
#include <filesystem>
#include <memory>

namespace PixieRenderer {
class IRenderer;
}

namespace PixieToolbox {

using namespace PixieRenderer;

class Scene;

class SceneLoader {
  public:
	static std::shared_ptr<Scene> LoadScene(std::filesystem::path path, std::shared_ptr<IRenderer> renderer);

	static std::shared_ptr<Scene> LoadFBX(const std::filesystem::path& path, std::shared_ptr<IRenderer> r);
	static std::shared_ptr<Scene> LoadGLTF(const std::filesystem::path& path, std::shared_ptr<IRenderer> r);
};

} // namespace PixieToolbox
