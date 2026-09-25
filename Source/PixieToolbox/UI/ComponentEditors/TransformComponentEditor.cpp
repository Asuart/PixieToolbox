#include "ComponentEditorRegistry.h"
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<TransformComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(TransformComponent& tc, InspectorContext&) {
		glm::vec3 pos = tc.transform.GetPosition();
		glm::vec3 scl = tc.transform.GetScale();
		glm::vec3 rot = glm::degrees(glm::eulerAngles(tc.transform.GetRotation()));

		bool changed = false;
		changed |= ImGui::DragFloat3("Position", &pos.x, 0.01f);
		changed |= ImGui::DragFloat3("Rotation", &rot.x, 0.5f);
		changed |= ImGui::DragFloat3("Scale", &scl.x, 0.01f, 0.0001f, 1000.0f);

		if (changed) {
			tc.transform.SetPosition(pos);
			tc.transform.SetRotation(glm::quat(glm::radians(rot)));
			tc.transform.SetScale(scl);
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::TransformComponent, "Transform");
