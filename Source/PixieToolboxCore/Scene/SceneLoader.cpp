#include "SceneLoader.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>

#include <ufbx.h>

#include <PixieRenderer/Image/Image2D.h>
#include <PixieRenderer/Material/PBRMaterial.h>
#include <PixieRenderer/Renderer/IRenderer.h>

#include "PixieToolboxCore/Scene/Components.h"
#include "PixieToolboxCore/Scene/Scene.h"
#include "PixieToolboxCore/Scripts/FreeCameraController.h"
#include "PixieToolboxCore/Texture/TextureLoader.h"

using namespace PixieRenderer;

namespace PixieToolbox {

namespace {

std::string UfbxStringToStd(const ufbx_string& s) {
	if (!s.data || s.length == 0)
		return {};
	return std::string(s.data, s.length);
}

glm::mat4 UfbxToGlm(const ufbx_matrix& m) {
	glm::mat4 r(1.0f);
	for (int c = 0; c < 3; ++c) {
		r[c] = glm::vec4(
		    static_cast<float>(m.cols[c].x),
		    static_cast<float>(m.cols[c].y),
		    static_cast<float>(m.cols[c].z),
		    0.0f
		);
	}
	r[3] = glm::
	    vec4(static_cast<float>(m.cols[3].x), static_cast<float>(m.cols[3].y), static_cast<float>(m.cols[3].z), 1.0f);
	return r;
}

std::filesystem::path ResolveTexturePath(ufbx_texture* tex, const std::filesystem::path& rootDir) {
	if (!tex)
		return {};

	std::error_code ec;
	const std::filesystem::path direct = UfbxStringToStd(tex->filename);
	if (!direct.empty() && std::filesystem::exists(direct, ec) && !ec)
		return direct;

	std::filesystem::path rel = UfbxStringToStd(tex->relative_filename);
	if (rel.empty())
		rel = direct;

	if (!rel.empty()) {
		const std::filesystem::path joined = rootDir / rel;
		if (std::filesystem::exists(joined, ec) && !ec)
			return joined;

		const std::filesystem::path byName = rootDir / rel.filename();
		if (std::filesystem::exists(byName, ec) && !ec)
			return byName;
	}
	return direct;
}

TextureHandle LoadTextureCached(
    std::shared_ptr<IRenderer> r,
    const std::filesystem::path& filePath,
    std::unordered_map<std::string, TextureHandle>& cache
) {
	if (filePath.empty())
		return {};

	const std::string key = filePath.string();
	auto it = cache.find(key);
	if (it != cache.end())
		return it->second;

	Texture<glm::vec4> tex = TextureLoader::LoadTextureFloatRGBA(filePath);
	TextureHandle h{};
	if (tex.GetPixelsCount() > 0) {
		h = r->CreateTexture(&tex.GetImage());
	} else {
		std::cerr << "[SceneLoader] Failed to load texture: " << key << "\n";
	}
	cache[key] = h;
	return h;
}

TextureHandle ResolveMaterialTexture(
    std::shared_ptr<IRenderer> r,
    ufbx_texture* tex,
    const std::filesystem::path& rootDir,
    std::unordered_map<std::string, TextureHandle>& cache
) {
	if (!tex)
		return {};
	return LoadTextureCached(r, ResolveTexturePath(tex, rootDir), cache);
}

} // namespace

std::shared_ptr<Scene> SceneLoader::LoadScene(std::filesystem::path path, std::shared_ptr<IRenderer> r) {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(path.stem().string());

	ufbx_load_opts opts{};
	opts.generate_missing_normals = true;
	opts.target_axes = ufbx_axes_right_handed_y_up;
	opts.target_unit_meters = 1.0f;

	ufbx_error error{};
	ufbx_scene* fbx = ufbx_load_file(path.string().c_str(), &opts, &error);
	if (!fbx) {
		std::cerr << "[SceneLoader] Failed to open FBX: " << path << "\n";
		if (error.description.data && error.description.length > 0)
			std::cerr << "  " << std::string(error.description.data, error.description.length) << "\n";
		return scene;
	}

	std::cout << "[SceneLoader] Opened " << path << "\n";

	const std::filesystem::path rootDir = path.parent_path();
	std::unordered_map<std::string, TextureHandle> textureCache;

	std::unordered_map<ufbx_material*, std::shared_ptr<IMaterial>> materialPtrMap;
	std::unordered_map<ufbx_material*, MaterialHandle> materialHandleMap;

	for (size_t i = 0; i < fbx->materials.count; ++i) {
		ufbx_material* fbxMat = fbx->materials.data[i];

		auto mat = std::make_shared<PBRMaterial>();

		const ufbx_vec4 bc = fbxMat->pbr.base_color.value_vec4;
		mat->SetAlbedo({ static_cast<float>(bc.x), static_cast<float>(bc.y), static_cast<float>(bc.z) });
		mat->SetMetallic(static_cast<float>(fbxMat->pbr.metalness.value_real));
		mat->SetRoughness(static_cast<float>(fbxMat->pbr.roughness.value_real));

		if (fbxMat->pbr.base_color.texture_enabled)
			mat->SetAlbedoTexture(ResolveMaterialTexture(r, fbxMat->pbr.base_color.texture, rootDir, textureCache));
		if (fbxMat->pbr.metalness.texture_enabled)
			mat->SetMetallicTexture(ResolveMaterialTexture(r, fbxMat->pbr.metalness.texture, rootDir, textureCache));
		if (fbxMat->pbr.roughness.texture_enabled)
			mat->SetRoughnessTexture(ResolveMaterialTexture(r, fbxMat->pbr.roughness.texture, rootDir, textureCache));
		if (fbxMat->pbr.normal_map.texture_enabled)
			mat->SetNormalTexture(ResolveMaterialTexture(r, fbxMat->pbr.normal_map.texture, rootDir, textureCache));

		MaterialHandle mh = r->CreateMaterial(mat.get());
		mat->SetHandle(mh);

		materialPtrMap[fbxMat] = mat;
		materialHandleMap[fbxMat] = mh;
	}

	std::shared_ptr<IMaterial> fallbackMaterialPtr;
	MaterialHandle fallbackMaterialHandle{};
	{
		auto mat = std::make_shared<PBRMaterial>();
		mat->SetAlbedo({ 1.0f, 1.0f, 1.0f });
		mat->SetMetallic(0.0f);
		mat->SetRoughness(0.8f);
		MaterialHandle mh = r->CreateMaterial(mat.get());
		mat->SetHandle(mh);
		fallbackMaterialPtr = mat;
		fallbackMaterialHandle = mh;
	}

	std::cout << "[SceneLoader] Materials: " << fbx->materials.count
	          << " (+1 fallback), textures cached: " << textureCache.size() << "\n";

	size_t entityCount = 0;

	for (size_t ni = 0; ni < fbx->nodes.count; ++ni) {
		ufbx_node* node = fbx->nodes.data[ni];
		if (!node->mesh || !node->visible)
			continue;

		ufbx_mesh* mesh = node->mesh;
		if (mesh->num_indices == 0 || mesh->num_triangles == 0)
			continue;

		auto meshData = std::make_shared<Mesh>();
		const size_t vcount = mesh->num_vertices;
		meshData->vertexes.resize(vcount);

		const bool hasNormal = mesh->vertex_normal.exists;
		const bool hasUV = mesh->vertex_uv.exists;

		for (size_t vi = 0; vi < vcount; ++vi) {
			Vertex v{};

			const ufbx_vec3 p = ufbx_get_vertex_vec3(&mesh->vertex_position, vi);
			v.position = { static_cast<float>(p.x), static_cast<float>(p.y), static_cast<float>(p.z) };

			if (hasNormal) {
				const ufbx_vec3 n = ufbx_get_vertex_vec3(&mesh->vertex_normal, vi);
				const glm::vec3 nn(static_cast<float>(n.x), static_cast<float>(n.y), static_cast<float>(n.z));
				v.normal = glm::length(nn) > 1e-8f ? glm::normalize(nn) : glm::vec3(0, 1, 0);
			} else {
				v.normal = { 0.0f, 1.0f, 0.0f };
			}

			if (hasUV) {
				const ufbx_vec2 uv = ufbx_get_vertex_vec2(&mesh->vertex_uv, vi);
				v.uv = { static_cast<float>(uv.x), static_cast<float>(uv.y) };
			}

			meshData->vertexes[vi] = v;
		}

		meshData->indexes.clear();
		meshData->indexes.reserve(mesh->num_triangles * 3);

		std::vector<uint32_t> triIndices(mesh->max_face_triangles * 3);

		for (size_t fi = 0; fi < mesh->faces.count; ++fi) {
			ufbx_face face = mesh->faces.data[fi];
			uint32_t numTris = ufbx_triangulate_face(triIndices.data(), triIndices.size(), mesh, face);

			const size_t corners = static_cast<size_t>(numTris) * 3;
			for (size_t i = 0; i < corners; ++i) {
				meshData->indexes.push_back(static_cast<int32_t>(triIndices[i]));
			}
		}

		MeshHandle meshHandle = r->CreateMesh(meshData.get());

		std::shared_ptr<IMaterial> materialPtr = fallbackMaterialPtr;
		MaterialHandle materialHandle = fallbackMaterialHandle;
		if (node->materials.count > 0) {
			auto itPtr = materialPtrMap.find(node->materials.data[0]);
			auto itHandle = materialHandleMap.find(node->materials.data[0]);
			if (itPtr != materialPtrMap.end() && itHandle != materialHandleMap.end()) {
				materialPtr = itPtr->second;
				materialHandle = itHandle->second;
			}
		}

		const std::string name = UfbxStringToStd(node->name);
		Scene::Entity e = scene->CreateEntity(name.empty() ? "Mesh" : name);

		scene->AddComponent<MeshComponent>(e, MeshComponent{ meshData, meshHandle });
		scene->AddComponent<MaterialComponent>(e, MaterialComponent{ materialPtr, materialHandle });
		scene->AddComponent<WorldMatrixComponent>(e, WorldMatrixComponent{ UfbxToGlm(node->geometry_to_world) });

		++entityCount;
	}

	std::cout << "[SceneLoader] Loaded " << entityCount << " mesh entit" << (entityCount == 1 ? "y" : "ies") << "\n";

	ufbx_free_scene(fbx);

	Scene::Entity cam = scene->CreateEntity("MainCamera");
	scene->AddComponent<CameraComponent>(cam);
	TransformComponent& tfc = scene->AddComponent<TransformComponent>(cam);

	const glm::vec3 camPos(0.0f, 1.5f, -4.0f);
	constexpr float yaw = glm::radians(-90.0f);
	constexpr float pitch = glm::radians(-5.0f);
	const glm::vec3 fwd(std::cos(pitch) * std::cos(yaw), std::sin(pitch), std::cos(pitch) * std::sin(yaw));
	tfc.transform.LookAt(camPos, camPos + fwd, glm::vec3(0, 1, 0));

	ScriptComponent& sc = scene->AddComponent<ScriptComponent>(cam);
	auto ctrl = std::make_unique<FreeCameraController>(30.0f, 0.2f);
	ctrl->captureCursor = false;
	sc.scripts.push_back(std::move(ctrl));

	return scene;
}

} // namespace PixieToolbox
