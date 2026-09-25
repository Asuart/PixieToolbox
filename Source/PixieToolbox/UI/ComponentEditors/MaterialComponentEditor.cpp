#include "ComponentEditorRegistry.h"

#include <typeinfo>

#include <PixieRenderer/Material/IMaterial.h>
#include <PixieRenderer/Material/PBRMaterial.h>
#include <PixieToolboxCore/Scene/Components.h>

namespace PixieToolbox::Detail {

using namespace PixieRenderer;

template <> struct ComponentEditor<MaterialComponent> {
	static constexpr bool kHasEditor = true;
	static void Draw(MaterialComponent& matc, InspectorContext&) {
		ImGui::Text("Handle: %s", matc.materialHandle ? "valid" : "null");
		if (!matc.material) {
			ImGui::TextDisabled("Material data is null");
			return;
		}

		ImGui::TextDisabled("Type: %s", typeid(*matc.material).name());

		if (auto* pbr = dynamic_cast<PBRMaterial*>(matc.material.get())) {
			glm::vec3 albedo = pbr->GetAlbedo();
			float metallic = pbr->GetMetallic();
			float roughness = pbr->GetRoughness();

			if (ImGui::ColorEdit3("Albedo", &albedo.x)) {
				pbr->SetAlbedo(albedo);
			}
			if (ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f)) {
				pbr->SetMetallic(metallic);
			}
			if (ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f)) {
				pbr->SetRoughness(roughness);
			}

			ImGui::Text(
			    "Textures: albedo=%s metal=%s rough=%s normal=%s",
			    pbr->GetAlbedoTexture() ? "y" : "n",
			    pbr->GetMetallicTexture() ? "y" : "n",
			    pbr->GetRoughnessTexture() ? "y" : "n",
			    pbr->GetNormalTexture() ? "y" : "n"
			);
		}
	}
};

} // namespace PixieToolbox::Detail

PIXIE_REGISTER_COMPONENT_EDITOR(PixieToolbox::MaterialComponent, "Material");
