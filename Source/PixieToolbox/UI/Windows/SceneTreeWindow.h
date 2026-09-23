#pragma once
#include <memory>

#include <PixieToolboxCore/Scene/Scene.h>

#include "PixieToolbox/UI/UIWindow.h"

namespace PixieToolbox {

class Scene;

class SceneTreeWindow : public UIWindow {
  public:
	SceneTreeWindow(UI* ui, std::shared_ptr<IRenderer> renderer);

	void SetScene(std::shared_ptr<Scene> scene);

	void Draw() override;

  private:
	void DrawEntityNode(Scene::Entity e, Scene::Entity& selected, Scene::Entity& toDelete);
	void DrawEntityComponents(Scene::Entity e, Scene::Entity& toDelete);

	std::shared_ptr<Scene> m_scene;
	Scene::Entity m_selected = Scene::Null;
};

} // namespace PixieToolbox
