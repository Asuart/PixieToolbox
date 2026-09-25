#pragma once
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <vector>

#include <entt/entt.hpp>

#include <imgui.h>

namespace PixieToolbox {

struct InspectorContext {
	entt::registry& registry;
	entt::entity entity;
};

namespace Detail {
template <typename T> struct ComponentEditor {
	static constexpr bool kHasEditor = false;
	static void Draw(T&, InspectorContext&) {
		ImGui::TextDisabled("%s — no custom editor", typeid(T).name());
	}
};
} // namespace Detail

class ComponentEditorRegistry {
  public:
	static ComponentEditorRegistry& Instance() {
		static ComponentEditorRegistry inst;
		return inst;
	}

	template <typename T> static void Register(const char* label) {
		Instance().m_entries[entt::type_hash<T>::value()] = Entry{
			label,
			[](entt::registry& reg, entt::entity e) { return reg.all_of<T>(e); },
			[](entt::registry& reg, entt::entity e, InspectorContext& ctx) {
			    Detail::ComponentEditor<T>::Draw(reg.get<T>(e), ctx);
			}
		};
	}

	static void DrawAll(entt::registry& reg, entt::entity e) {
		Instance().DrawAllImpl(reg, e);
	}

  private:
	struct Entry {
		const char* label;
		std::function<bool(entt::registry&, entt::entity)> has;
		std::function<void(entt::registry&, entt::entity, InspectorContext&)> draw;
	};

	std::unordered_map<entt::id_type, Entry> m_entries;

	void DrawAllImpl(entt::registry& reg, entt::entity e) {
		InspectorContext ctx{ reg, e };
		for (const auto& [key, entry] : m_entries) {
			if (!entry.has(reg, e)) {
				continue;
			} else {
				ImGui::CollapsingHeader(entry.label, ImGuiTreeNodeFlags_DefaultOpen);
				entry.draw(reg, e, ctx);
			}
		}
	}
};

} // namespace PixieToolbox

#define PIXIE_CONCAT_IMPL(a, b) a##b
#define PIXIE_CONCAT(a, b) PIXIE_CONCAT_IMPL(a, b)

#define PIXIE_REGISTER_COMPONENT_EDITOR(Type, Label)                                                                   \
	namespace {                                                                                                        \
	[[maybe_unused]] const bool PIXIE_CONCAT(pixie_reg_editor_, __LINE__) = [] {                                       \
		::PixieToolbox::ComponentEditorRegistry::Register<Type>(Label);                                                \
		return true;                                                                                                   \
	}();                                                                                                               \
	}
