#include "SceneTreeWindow.h"

#include <imgui.h>

#include <cstring>
#include <string>

#include <PixieToolboxCore/Scene/Components.h>
#include <PixieToolboxCore/Scene/Scene.h>

#include <PixieRenderer/Material/IMaterial.h>
#include <PixieRenderer/Material/PBRMAterial.h>
#include <PixieRenderer/Mesh/Mesh.h>

namespace PixieToolbox {

namespace {

const char* BoolStr(bool v) {
	return v ? "true" : "false";
}

// Помощник: возвращает валидное имя или placeholder.
std::string SafeName(const std::string& s) {
	return s.empty() ? "<unnamed>" : s;
}

} // namespace

SceneTreeWindow::SceneTreeWindow(UI* ui, std::shared_ptr<IRenderer> renderer) : UIWindow(ui, renderer) {
}

void SceneTreeWindow::SetScene(std::shared_ptr<Scene> scene) {
	m_scene = std::move(scene);
	m_selected = Scene::Null;
}

void SceneTreeWindow::Draw() {
	ImGui::SetNextWindowSize(ImVec2(700, 500), ImGuiCond_FirstUseEver);
	if (!ImGui::Begin("Scene")) {
		ImGui::End();
		return;
	}

	if (!m_scene) {
		ImGui::TextDisabled("No scene loaded");
		ImGui::End();
		return;
	}

	Scene::Entity toDelete = Scene::Null;

	// --- Левая панель: дерево entity ---
	ImGui::BeginChild("##tree", ImVec2(280.0f, 0.0f), true);

	Scene::Entity root = m_scene->GetRoot();
	if (root != Scene::Null) {
		DrawEntityNode(root, m_selected, toDelete);
	} else {
		ImGui::TextDisabled("Scene has no root");
	}

	ImGui::EndChild();

	ImGui::SameLine();

	// --- Правая панель: компоненты выбранного entity ---
	ImGui::BeginChild("##components", ImVec2(0.0f, 0.0f), true);
	if (m_selected != Scene::Null && m_scene->Registry().valid(m_selected)) {
		DrawEntityComponents(m_selected, toDelete);
	} else {
		ImGui::TextDisabled("Select an entity");
	}
	ImGui::EndChild();

	// Удаление — после обхода, чтобы не портить итерацию.
	if (toDelete != Scene::Null) {
		if (m_selected == toDelete)
			m_selected = Scene::Null;
		m_scene->DestroyEntity(toDelete);
	}

	ImGui::End();
}

void SceneTreeWindow::DrawEntityNode(Scene::Entity e, Scene::Entity& selected, Scene::Entity& toDelete) {
	auto& reg = m_scene->Registry();

	auto* name = reg.try_get<NameComponent>(e);
	const std::string label = name ? SafeName(name->name) : "<entity " + std::to_string(entt::to_integral(e)) + ">";

	auto* hier = reg.try_get<HierarchyComponent>(e);
	const bool hasChildren = hier && !hier->children.empty();

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                           ImGuiTreeNodeFlags_SpanAvailWidth;
	if (!hasChildren)
		flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
	if (e == selected)
		flags |= ImGuiTreeNodeFlags_Selected;

	// Уникальный ID (у разных entity может быть одинаковое имя).
	ImGui::PushID(static_cast<int>(entt::to_integral(e)));

	const bool open = ImGui::TreeNodeEx("##node", flags, "%s", label.c_str());

	if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
		selected = e;
	}

	// Контекстное меню
	if (ImGui::BeginPopupContextItem("##ctx")) {
		if (ImGui::MenuItem("Select"))
			selected = e;
		if (ImGui::MenuItem("Delete", "Del", false, e != m_scene->GetRoot()))
			toDelete = e;
		ImGui::EndPopup();
	}

	if (open && hasChildren) {
		// Копия — на случай если во время отрисовки дети удалятся.
		std::vector<Scene::Entity> children = hier->children;
		for (Scene::Entity c : children) {
			if (reg.valid(c))
				DrawEntityNode(c, selected, toDelete);
		}
		ImGui::TreePop();
	}

	ImGui::PopID();
}

void SceneTreeWindow::DrawEntityComponents(Scene::Entity e, Scene::Entity& toDelete) {
	auto& reg = m_scene->Registry();

	if (!reg.valid(e)) {
		ImGui::TextDisabled("Entity is invalid");
		return;
	}

	// Заголовок
	if (auto* name = reg.try_get<NameComponent>(e)) {
		char buf[256];
		std::snprintf(buf, sizeof(buf), "%s", name->name.c_str());
		if (ImGui::InputText("Name", buf, sizeof(buf))) {
			name->name = buf;
		}
	}

	ImGui::Text("Entity ID: %u", static_cast<uint32_t>(entt::to_integral(e)));
	ImGui::Separator();

	if (ImGui::Button("Delete entity", ImVec2(-1, 0))) {
		if (e != m_scene->GetRoot())
			toDelete = e;
	}
	ImGui::Separator();

	// --- TransformComponent ---
	if (auto* tc = reg.try_get<TransformComponent>(e)) {
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			glm::vec3 pos = tc->transform.GetPosition();
			glm::vec3 scl = tc->transform.GetScale();

			const glm::quat q = tc->transform.GetRotation();
			glm::vec3 rot = glm::degrees(glm::eulerAngles(q));

			bool changed = false;
			changed |= ImGui::DragFloat3("Position", &pos.x, 0.01f);
			changed |= ImGui::DragFloat3("Rotation", &rot.x, 0.5f);
			changed |= ImGui::DragFloat3("Scale", &scl.x, 0.01f, 0.0001f, 1000.0f);

			if (ImGui::IsItemDeactivatedAfterEdit() || changed) {
				tc->transform.SetPosition(pos);
				tc->transform.SetRotation(glm::quat(glm::radians(rot)));
				tc->transform.SetScale(scl);
			}
		}
	}

	// --- WorldMatrixComponent ---
	if (auto* wm = reg.try_get<WorldMatrixComponent>(e)) {
		if (ImGui::CollapsingHeader("World Matrix")) {
			const glm::mat4& m = wm->matrix;
			for (int row = 0; row < 4; ++row) {
				ImGui::Text("[%7.3f %7.3f %7.3f %7.3f]", m[0][row], m[1][row], m[2][row], m[3][row]);
			}
		}
	}

	// --- MeshComponent ---
	if (auto* mc = reg.try_get<MeshComponent>(e)) {
		if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Handle: %s", mc->meshHandle ? "valid" : "null");
			if (mc->mesh) {
				ImGui::Text("Vertices: %zu", mc->mesh->vertexes.size());
				ImGui::Text("Indices:  %zu", mc->mesh->indexes.size());
			} else {
				ImGui::TextDisabled("Mesh data is null");
			}
		}
	}

	// --- MaterialComponent ---
	if (auto* matc = reg.try_get<MaterialComponent>(e)) {
		if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Text("Handle: %s", matc->materialHandle ? "valid" : "null");
			if (matc->material) {
				// IMaterial имеет только source-строки, конкретику смотри через PBRMaterial.
				ImGui::TextDisabled("Material type: %s", typeid(*matc->material).name());

				if (auto* pbr = dynamic_cast<PBRMaterial*>(matc->material.get())) {
					glm::vec3 albedo = pbr->GetAlbedo();
					float metallic = pbr->GetMetallic();
					float roughness = pbr->GetRoughness();

					if (ImGui::ColorEdit3("Albedo", &albedo.x))
						pbr->SetAlbedo(albedo);
					if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f))
						pbr->SetMetallic(metallic);
					if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f))
						pbr->SetRoughness(roughness);

					ImGui::Text(
					    "Textures: albedo=%s metal=%s rough=%s normal=%s",
					    pbr->GetAlbedoTexture() ? "y" : "n",
					    pbr->GetMetallicTexture() ? "y" : "n",
					    pbr->GetRoughnessTexture() ? "y" : "n",
					    pbr->GetNormalTexture() ? "y" : "n"
					);
				}
			}
		}
	}

	// --- CameraComponent ---
	if (auto* cc = reg.try_get<CameraComponent>(e)) {
		if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
			float fovy = cc->camera.GetFieldOfViewY();
			float aspect = cc->camera.GetAspect();
			float znear = cc->camera.GetNear();
			float zfar = cc->camera.GetFar();

			if (ImGui::SliderAngle("FOV Y", &fovy, 1.0f, 179.0f))
				cc->camera.SetFieldOfViewY(fovy);
			if (ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f))
				cc->camera.SetAspect(aspect);
			if (ImGui::DragFloat("Near", &znear, 0.001f, 0.0001f, 10.0f))
				cc->camera.SetNear(znear);
			if (ImGui::DragFloat("Far", &zfar, 1.0f, znear, 100000.0f))
				cc->camera.SetFar(zfar);
		}
	}

	// --- ScriptComponent ---
	if (auto* sc = reg.try_get<ScriptComponent>(e)) {
		if (ImGui::CollapsingHeader("Scripts")) {
			ImGui::Text("Count: %zu", sc->scripts.size());
			for (size_t i = 0; i < sc->scripts.size(); ++i) {
				const auto* s = sc->scripts[i].get();
				ImGui::BulletText("[%zu] %s", i, s ? typeid(*s).name() : "<null>");
			}
		}
	}

	// --- HierarchyComponent ---
	if (auto* h = reg.try_get<HierarchyComponent>(e)) {
		if (ImGui::CollapsingHeader("Hierarchy")) {
			auto parentName = [&](Scene::Entity p) -> std::string {
				if (p == Scene::Null)
					return "<none>";
				if (auto* n = reg.try_get<NameComponent>(p))
					return SafeName(n->name);
				return "<entity " + std::to_string(entt::to_integral(p)) + ">";
			};

			ImGui::Text("Parent: %s", parentName(h->parent).c_str());
			ImGui::Text("Children: %zu", h->children.size());
			for (size_t i = 0; i < h->children.size(); ++i) {
				ImGui::BulletText("[%zu] %s", i, parentName(h->children[i]).c_str());
			}
		}
	}

	// --- SphereComponent ---
	if (auto* sc = reg.try_get<SphereComponent>(e)) {
		if (ImGui::CollapsingHeader("Sphere")) {
			ImGui::DragFloat("Radius", &sc->radius, 0.01f, 0.001f, 1000.0f);
		}
	}
}

} // namespace PixieToolbox
