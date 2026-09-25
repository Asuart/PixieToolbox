#include "ComponentEditorRegistry.h"
#include <PixieRenderer/Mesh/Mesh.h>
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<MeshComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(MeshComponent& mc, InspectorContext&) {
		ImGui::Text("Handle: %s", mc.meshHandle ? "valid" : "null");
		if (mc.mesh) {
			ImGui::Text("Vertices: %zu", mc.mesh->vertexes.size());
			ImGui::Text("Indices:  %zu", mc.mesh->indexes.size());
		} else {
			ImGui::TextDisabled("Mesh data is null");
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::MeshComponent, "Mesh");
