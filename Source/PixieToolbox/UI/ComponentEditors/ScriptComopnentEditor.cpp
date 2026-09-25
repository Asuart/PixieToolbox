#include "ComponentEditorRegistry.h"

#include <typeinfo>

#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<ScriptComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(ScriptComponent& sc, InspectorContext&) {
		ImGui::Text("Count: %zu", sc.scripts.size());
		for (size_t i = 0; i < sc.scripts.size(); ++i) {
			const auto* s = sc.scripts[i].get();
			ImGui::BulletText("[%zu] %s", i, s ? typeid(*s).name() : "<null>");
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::ScriptComponent, "Scripts");
