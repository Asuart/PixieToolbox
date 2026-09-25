#include "InspectorWindow.h"

#include <imgui.h>

#include <cstdio>
#include <string>

#include <PixieToolboxCore/Scene/Components.h>
#include <PixieToolboxCore/Scene/Scene.h>

#include <PixieRenderer/Material/IMaterial.h>
#include <PixieRenderer/Material/PBRMaterial.h>
#include <PixieRenderer/Mesh/Mesh.h>

#include "PixieToolbox/Scene/SceneSelection.h"
#include "PixieToolbox/UI/ComponentEditors/ComponentEditorRegistry.h"

namespace PixieToolbox {

namespace {

std::string SafeName(const std::string& s) {
	return s.empty() ? "<unnamed>" : s;
}

} // namespace

InspectorWindow::InspectorWindow(UI* ui, std::shared_ptr<IRenderer> renderer, std::shared_ptr<SceneSelection> selection)
    : UIWindow(ui, renderer), m_selection(std::move(selection)) {
}

void InspectorWindow::SetScene(std::shared_ptr<Scene> scene) {
	m_scene = std::move(scene);
	if (m_selection) {
		m_selection->selected = Scene::Null;
	}
}

void InspectorWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(420, 500), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Inspector")) {
		ImGui::End();
		return;
	}

	if (!m_scene || !m_selection) {
		ImGui::TextDisabled("No scene loaded");
		ImGui::End();
		return;
	}

	const Scene::Entity e = m_selection->selected;
	if (e == Scene::Null || !m_scene->Registry().valid(e)) {
		ImGui::TextDisabled("Select an entity");
		ImGui::End();
		return;
	}

    auto& reg = m_scene->Registry();

	DrawEntityHeader(e);

	ImGui::Separator();
	
	Scene::Entity toDelete = Scene::Null;
	DrawEntityActions(e, toDelete);
	if (toDelete != Scene::Null) {
		m_selection->selected = Scene::Null;
		m_scene->DestroyEntity(toDelete);
	}

	ImGui::Separator();

	ComponentEditorRegistry::DrawAll(reg, e);

	ImGui::End();
}

void InspectorWindow::DrawEntityHeader(Scene::Entity e) {
	auto& reg = m_scene->Registry();

	if (auto* name = reg.try_get<NameComponent>(e)) {
		char buf[256];
		std::snprintf(buf, sizeof(buf), "%s", name->name.c_str());
		if (ImGui::InputText("Name", buf, sizeof(buf))) {
			name->name = buf;
		}
	} else {
		ImGui::TextDisabled("<no NameComponent>");
	}

	const bool isRoot = (e == m_scene->GetRoot());
	ImGui::Text("Entity ID: %u%s", static_cast<uint32_t>(entt::to_integral(e)), isRoot ? "  (root)" : "");
}

void InspectorWindow::DrawEntityActions(Scene::Entity e, Scene::Entity& toDelete) {
	const bool isRoot = (e == m_scene->GetRoot());

	ImGui::BeginDisabled(isRoot);
	if (ImGui::Button("Delete entity", ImVec2(-1, 0))) {
		toDelete = e;
	}
	ImGui::EndDisabled();

	if (isRoot) {
		ImGui::TextDisabled("Root cannot be deleted");
	}
}

} // namespace PixieToolbox
