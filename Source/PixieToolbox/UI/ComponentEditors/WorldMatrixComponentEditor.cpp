#include "ComponentEditorRegistry.h"
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

template <> struct ComponentEditor<WorldMatrixComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(WorldMatrixComponent& wm, InspectorContext&) {
		const glm::mat4& m = wm.matrix;
		for (int row = 0; row < 4; ++row) {
			ImGui::Text("[%7.3f %7.3f %7.3f %7.3f]", m[0][row], m[1][row], m[2][row], m[3][row]);
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::WorldMatrixComponent, "World Matrix");
