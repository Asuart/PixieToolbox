#include "Scene.h"
#include "SceneObject.h"

#include <memory>
#include <string>
#include <string_view>
#include <memory>

#include <glaze/glaze.hpp>

Scene::Scene(const std::string& name) {
	_data.name = name;
	_data.m_rootObject = std::make_shared<SceneObject>("root");
}

Scene::~Scene() {
}

const std::string& Scene::GetName() const {
	return _data.name;
}

void Scene::SetName(const std::string& name) {
	_data.name = name;
}

std::shared_ptr<SceneObject> Scene::FindObject(const std::string& objectName) const {
	return _data.m_rootObject->FindObject(objectName);
}

std::vector<std::shared_ptr<SceneObject>> Scene::FindObjects(const std::string& objectName) const {
	return _data.m_rootObject->FindObjects(objectName);
}

std::shared_ptr<SceneObject> Scene::GetRootObject() const {
	return _data.m_rootObject;
}

void Scene::Start() {
	_data.m_rootObject->OnStart();
}

void Scene::Update() {
	_data.m_rootObject->OnUpdate();
}

void Scene::FixedUpdate() {
	_data.m_rootObject->OnFixedUpdate();
}

void Scene::AddObject(std::shared_ptr<SceneObject> object, std::shared_ptr<SceneObject> parent) {
	if (object == _data.m_rootObject) return;
	if (!parent) {
		parent = _data.m_rootObject;
	}
	object->SetParent(parent);
}

std::shared_ptr<SceneObject> Scene::CreateObject(const std::string& name, std::shared_ptr<SceneObject> parent) {
	if (!parent) parent = _data.m_rootObject;
	std::shared_ptr<SceneObject> object = std::make_shared<SceneObject>(name);
    object->SetParent(parent);
	return object;
}

void Scene::RemoveObject(const std::string& objectName) {
	std::shared_ptr<SceneObject> object = FindObject(objectName);
	RemoveObject(object);
}

void Scene::RemoveObjects(const std::string& objectName) {
	std::vector<std::shared_ptr<SceneObject>> objects = FindObjects(objectName);
	RemoveObjects(objects);
}

void Scene::RemoveObject(const std::shared_ptr<SceneObject> object) {
	if (object == _data.m_rootObject) return;
	object->SetParent(nullptr);
}

void Scene::RemoveObjects(const std::vector<std::shared_ptr<SceneObject>>& objects) {
	for (size_t i = 0; i < objects.size(); i++) {
		RemoveObject(objects[i]);
	}
}

// template <typename... Args>
// struct glz::meta<std::function<Args...>> {
//     static constexpr auto value = glz::skip{}; 
// };

// template <> struct glz::meta<SceneObject> {
//     using T = SceneObject;
//     static constexpr auto value = glz::object(
//         "name", &T::m_name,
//         // "components", &T::m_components,
//         "children", &T::m_children
//     );
// };

// namespace glz {
// 	template<>
// 	struct meta<SceneObject::Data> {
// 		using T = SceneObject::Data;
// 		static constexpr auto modify = object(
// 			"parent", [](auto& self) { 
//                 ResourceManager* mngr = ResourceManager::getInstance();
//                 ::std::shared_ptr<SceneObject> sharedResult = nullptr;
//                 if (self.parent == nullptr) return sharedResult;
//                 SceneObject* result = mngr->getResource<SceneObject>(self.parent->getName());
//                 if (result == nullptr) {
//                     result = mngr->loadResource<SceneObject>(self.parent->getPath());
//                 }
//                 sharedResult.reset(result);
//                 return sharedResult;
//             }
// 		);
// 	};
// }

// namespace glz {
//     template<>
//     struct meta<SceneObject> {
//        static constexpr auto modify = glz::object(
//             "m_parent", [](auto& self) { return glz::skip{}; }
//         );
//     };
// }

// namespace glz {
//     template<>
//     struct meta<SceneObject> {
//        static constexpr auto modify = glz::object(
//             "m_parent", glz::hide{&SceneObject::m_parent}
//         );
//     };
// }

std::vector<std::byte> Scene::serialize() {
	std::vector<std::byte> buffer;
	auto err = glz::write_json(_data, buffer);
	if (err) {
		std::cout << "Error: " << glz::format_error(err, buffer) << '\n';
		return {};
	}

    std::cout << std::string_view((char*)buffer.data(), buffer.size()) << "\n";

	return buffer;
}

Scene* Scene::deserialize(const std::vector<std::byte>&) {
	return new Scene();
}
