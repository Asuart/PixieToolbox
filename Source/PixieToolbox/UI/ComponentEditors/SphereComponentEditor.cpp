#include "ComponentEditorRegistry.h"
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<SphereComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(SphereComponent& sc, InspectorContext&) {
		ImGui::DragFloat("Radius", &sc.radius, 0.01f, 0.001f, 1000.0f);
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::SphereComponent, "Sphere");
