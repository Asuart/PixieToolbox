#include "ComponentEditorRegistry.h"

#include <string>

#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<HierarchyComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(HierarchyComponent& h, InspectorContext& ctx) {
		auto nameOf = [&](entt::entity p) -> std::string {
			if (p == entt::null) {
				return "<none>";
			}
			if (auto* n = ctx.registry.try_get<NameComponent>(p)) {
				return n->name.empty() ? "<unnamed>" : n->name;
			}
			return "<entity " + std::to_string(entt::to_integral(p)) + ">";
		};

		ImGui::Text("Parent: %s", nameOf(h.parent).c_str());
		ImGui::Text("Children: %zu", h.children.size());
		for (size_t i = 0; i < h.children.size(); ++i) {
			ImGui::BulletText("[%zu] %s", i, nameOf(h.children[i]).c_str());
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::HierarchyComponent, "Hierarchy");
