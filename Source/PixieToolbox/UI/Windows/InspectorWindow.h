#pragma once
#include <memory>

#include <PixieToolboxCore/Scene/Scene.h>

#include "PixieToolbox/UI/UIWindow.h"

namespace PixieToolbox {

class Scene;
struct SceneSelection;

class InspectorWindow : public UIWindow {
  public:
	InspectorWindow(UI* ui, std::shared_ptr<IRenderer> renderer, std::shared_ptr<SceneSelection> selection);

	void SetScene(std::shared_ptr<Scene> scene);

	void Draw() override;

  private:
	std::shared_ptr<Scene> m_scene;
	std::shared_ptr<SceneSelection> m_selection;

	void DrawEntityHeader(Scene::Entity e);
	void DrawEntityActions(Scene::Entity e, Scene::Entity& toDelete);
};

} // namespace PixieToolbox
