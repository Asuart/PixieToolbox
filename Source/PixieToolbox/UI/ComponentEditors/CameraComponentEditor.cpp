#include "ComponentEditorRegistry.h"
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<CameraComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(CameraComponent& cc, InspectorContext&) {
		float fovy = cc.camera.GetFieldOfViewY();
		float aspect = cc.camera.GetAspect();
		float znear = cc.camera.GetNear();
		float zfar = cc.camera.GetFar();

		if (ImGui::SliderAngle("FOV Y", &fovy, 1.0f, 179.0f)) cc.camera.SetFieldOfViewY(fovy);
		if (ImGui::DragFloat("Aspect", &aspect, 0.01f, 0.1f, 10.0f)) cc.camera.SetAspect(aspect);
		if (ImGui::DragFloat("Near", &znear, 0.001f, 0.0001f, 10.0f)) cc.camera.SetNear(znear);
		if (ImGui::DragFloat("Far", &zfar, 1.0f, znear, 100000.0f)) cc.camera.SetFar(zfar);
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::CameraComponent, "Camera");
