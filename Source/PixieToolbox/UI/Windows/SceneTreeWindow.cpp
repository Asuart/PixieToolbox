#include "SceneTreeWindow.h"

#include <imgui.h>

#include <string>
#include <vector>

#include <PixieToolboxCore/Scene/Components.h>
#include <PixieToolboxCore/Scene/Scene.h>

#include "PixieToolbox/Scene/SceneSelection.h"

namespace PixieToolbox {

namespace {

std::string SafeName(const std::string& s) {
	return s.empty() ? "<unnamed>" : s;
}

} // namespace

SceneTreeWindow::SceneTreeWindow(UI* ui, std::shared_ptr<IRenderer> renderer, std::shared_ptr<SceneSelection> selection)
    : UIWindow(ui, renderer), m_selection(std::move(selection)) {
}

void SceneTreeWindow::SetScene(std::shared_ptr<Scene> scene) {
	m_scene = std::move(scene);
	if (m_selection) {
		m_selection->selected = Scene::Null;
	}
}

void SceneTreeWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(320, 500), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Scene Tree")) {
		ImGui::End();
		return;
	}

	if (!m_scene || !m_selection) {
		ImGui::TextDisabled("No scene loaded");
		ImGui::End();
		return;
	}

	Scene::Entity toDelete = Scene::Null;

	const Scene::Entity root = m_scene->GetRoot();
	if (root != Scene::Null) {
		DrawEntityNode(root, toDelete);
	} else {
		ImGui::TextDisabled("Scene has no root");
	}

	if (toDelete != Scene::Null) {
		if (m_selection->selected == toDelete) {
			m_selection->selected = Scene::Null;
		}
		m_scene->DestroyEntity(toDelete);
	}

	ImGui::End();
}

void SceneTreeWindow::DrawEntityNode(Scene::Entity e, Scene::Entity& toDelete) {
	auto& reg = m_scene->Registry();

	auto* name = reg.try_get<NameComponent>(e);
	const std::string label = name ? SafeName(name->name) : "<entity " + std::to_string(entt::to_integral(e)) + ">";

	auto* hier = reg.try_get<HierarchyComponent>(e);
	const bool hasChildren = hier && !hier->children.empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                           ImGuiTreeNodeFlags_SpanAvailWidth;
	if (!hasChildren) {
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	}
	if (e == m_selection->selected) {
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushID(static_cast<int>(entt::to_integral(e)));

	const bool open = ImGui::TreeNodeEx("##node", flags, "%s", label.c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		m_selection->selected = e;
	}

	if (ImGui::BeginPopupContextItem("##ctx")) {
		if (ImGui::MenuItem("Select")) {
			m_selection->selected = e;
		}
		if (ImGui::MenuItem("Delete", "Del", false, e != m_scene->GetRoot())) {
			toDelete = e;
		}
		ImGui::EndPopup();
	}

	if (open && hasChildren) {
		std::vector<Scene::Entity> children = hier->children;
		for (Scene::Entity c : children) {
			if (reg.valid(c)) {
				DrawEntityNode(c, toDelete);
			}
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

} // namespace PixieToolbox
