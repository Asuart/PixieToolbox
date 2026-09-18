#include "ImGuiBackend.h"

#include "ImGuiBackendVulkan.h"

namespace PixieToolbox {

std::unique_ptr<ImGuiBackend> ImGuiBackend::Create(RenderAPI api) {
	switch (api) {
	case RenderAPI::Vulkan:
		return std::make_unique<ImGuiBackendVulkan>();
	default:
		return nullptr;
	}
}

} // namespace PixieToolbox
