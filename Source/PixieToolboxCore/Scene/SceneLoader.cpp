#include "SceneLoader.h"

namespace PixieToolbox {

std::shared_ptr<Scene> SceneLoader::LoadScene(std::filesystem::path path, std::shared_ptr<IRenderer> r) {
	const std::string ext = path.extension().string();
	if (ext == ".gltf" || ext == ".glb") {
		return LoadGLTF(path, r);
	}
	return LoadFBX(path, r);
}

} // namespace PixieToolbox
