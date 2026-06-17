#include "SceneTreeWindow.h"
/*
SceneTreeWindow::SceneTreeWindow(PixieEngineEditor& app, Interface& inter) :
	InterfaceWindow(app, inter) {}

void SceneTreeWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	if (ImGui::Begin("Scene", 0)) {
		if (ImGui::IsWindowFocused()) {
			HandleUserInput();
		}
		std::shared_ptr<Scene> scene = SceneManager::GetScene();
		if (scene.get()) {
			DrawSceneTree(scene->GetRootObject());
		}
	}
	ImGui::End();
}

void SceneTreeWindow::DrawSceneTree(SceneObject* object) {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_DefaultOpen;
	if (object->GetChildren().size() == 0) flags |= ImGuiTreeNodeFlags_Leaf;
	if (object == SceneManager::GetSelectedObject()) flags |= ImGuiTreeNodeFlags_Selected;
	bool openTreeNode = ImGui::TreeNodeEx(object->GetName().c_str(), flags);
	if (ImGui::IsItemClicked()) {
		SceneManager::SetSelectedObject(object);
	}
	if (openTreeNode) {
		for (size_t i = 0; i < object->GetChildren().size(); i++) {
			DrawSceneTree(object->GetChild((int32_t)i));
		}
		ImGui::TreePop();
	}
}

void SceneTreeWindow::HandleUserInput() {
	if (UserInput::GetKey(SDLK_DELETE)) {
		SceneManager::RemoveSelectedObject();
	}
}
*/