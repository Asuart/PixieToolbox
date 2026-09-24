#include "SceneLoader.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <format>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <tiny_gltf_v3.h>

#include <PixieRenderer/Image/Image2D.h>
#include <PixieRenderer/Material/PBRMaterial.h>
#include <PixieRenderer/Mesh/Mesh.h>
#include <PixieRenderer/Renderer/IRenderer.h>

#include "PixieToolboxCore/LogCategories.h"
#include "PixieToolboxCore/Scene/Components.h"
#include "PixieToolboxCore/Scene/Scene.h"
#include "PixieToolboxCore/Scripts/FreeCameraController.h"
#include "PixieToolboxCore/Texture/TextureLoader.h"

using namespace PixieRenderer;

namespace PixieToolbox {

namespace {

constexpr std::size_t kNoIndex = static_cast<std::size_t>(-1);

struct FallbackTextures {
	TextureHandle albedo;
	TextureHandle metallic;
	TextureHandle roughness;
	TextureHandle normal;
};

Image2D MakeSolidImage(glm::vec4 color) {
	Image2D img;
	img.resolution = glm::ivec2(1, 1);
	img.format = TextureFormat::RGBA32f;
	img.pixels.resize(sizeof(float) * 4);
	float* p = reinterpret_cast<float*>(img.pixels.data());
	p[0] = color.r;
	p[1] = color.g;
	p[2] = color.b;
	p[3] = color.a;
	return img;
}

TextureHandle CreateSolidTexture(const std::shared_ptr<IRenderer>& r, glm::vec4 color) {
	Image2D img = MakeSolidImage(color);
	return r->CreateTexture(&img);
}

FallbackTextures CreateFallbackTextures(const std::shared_ptr<IRenderer>& r) {
	FallbackTextures fb{};
	fb.albedo = CreateSolidTexture(r, { 1.0f, 1.0f, 1.0f, 1.0f });
	fb.metallic = CreateSolidTexture(r, { 1.0f, 1.0f, 1.0f, 1.0f });
	fb.roughness = CreateSolidTexture(r, { 1.0f, 1.0f, 1.0f, 1.0f });
	fb.normal = CreateSolidTexture(r, { 0.5f, 0.5f, 1.0f, 1.0f });
	return fb;
}

TextureHandle ResolveOrFallback(TextureHandle h, TextureHandle fallback) {
	return h ? h : fallback;
}

std::string ToStr(tg3_str s) {
	if (!s.data || s.len == 0) {
		return {};
	}
	return std::string(s.data, s.len);
}

bool IsValidIndex(int32_t idx, uint32_t count) {
	return idx >= 0 && static_cast<uint32_t>(idx) < count;
}

bool IsValidIndexU(uint32_t idx, uint32_t count) {
	return idx < count;
}

bool GetAccessorLayout(
    const tg3_model& model,
    const tg3_accessor& acc,
    const uint8_t*& outData,
    std::size_t& outStride,
    std::size_t& outBase,
    std::size_t& outAvailable
) {
	if (!IsValidIndex(acc.buffer_view, model.buffer_views_count)) {
		return false;
	}

	const tg3_buffer_view& bv = model.buffer_views[acc.buffer_view];
	if (!IsValidIndex(bv.buffer, model.buffers_count)) {
		return false;
	}

	const tg3_buffer& buf = model.buffers[bv.buffer];
	if (!buf.data.data || buf.data.count == 0) {
		return false;
	}

	const int32_t compSize = tg3_component_size(acc.component_type);
	const int32_t numComp = tg3_num_components(acc.type);
	if (compSize <= 0 || numComp <= 0) {
		return false;
	}

	const std::size_t elemSize = static_cast<std::size_t>(compSize) * static_cast<std::size_t>(numComp);
	outStride = (bv.byte_stride > 0) ? bv.byte_stride : elemSize;
	outBase = static_cast<std::size_t>(bv.byte_offset) + static_cast<std::size_t>(acc.byte_offset);
	outData = buf.data.data;
	outAvailable = static_cast<std::size_t>(buf.data.count);

	return true;
}

template <typename T, int N>
bool ReadVecAttribute(
    const tg3_model& model,
    const tg3_accessor& acc,
    std::vector<glm::vec<N, T, glm::defaultp>>& out
) {
	const uint8_t* data = nullptr;
	std::size_t stride = 0, base = 0, avail = 0;
	if (!GetAccessorLayout(model, acc, data, stride, base, avail)) {
		return false;
	}
	if (acc.component_type != TG3_COMPONENT_TYPE_FLOAT) {
		return false;
	}

	const std::size_t elemSize = sizeof(T) * N;
	out.resize(acc.count);
	for (std::size_t i = 0; i < acc.count; ++i) {
		const std::size_t off = base + i * stride;
		if (off + elemSize > avail) {
			return false;
		}
		T tmp[N];
		std::memcpy(tmp, data + off, elemSize);
		for (int c = 0; c < N; ++c) {
			out[i][c] = tmp[c];
		}
	}
	return true;
}

template <typename TSrc, typename TDst>
bool ReadScalarIndices(const tg3_model& model, const tg3_accessor& acc, std::vector<TDst>& out) {
	const uint8_t* data = nullptr;
	std::size_t stride = 0, base = 0, avail = 0;
	if (!GetAccessorLayout(model, acc, data, stride, base, avail)) {
		return false;
	}

	out.resize(acc.count);
	for (std::size_t i = 0; i < acc.count; ++i) {
		const std::size_t off = base + i * stride;
		if (off + sizeof(TSrc) > avail) {
			return false;
		}
		TSrc tmp{};
		std::memcpy(&tmp, data + off, sizeof(TSrc));
		out[i] = static_cast<TDst>(tmp);
	}

	return true;
}

glm::mat4 NodeTransform(const tg3_node& node) {
	if (node.has_matrix) {
		glm::mat4 m;
		float* p = glm::value_ptr(m);
		for (int i = 0; i < 16; ++i) {
			p[i] = static_cast<float>(node.matrix[i]);
		}
		return m;
	}

	glm::mat4 m(1.0f);
	m = glm::translate(
	    m,
	    glm::vec3(
	        static_cast<float>(node.translation[0]),
	        static_cast<float>(node.translation[1]),
	        static_cast<float>(node.translation[2])
	    )
	);

	const glm::quat
	    q(static_cast<float>(node.rotation[3]),
	      static_cast<float>(node.rotation[0]),
	      static_cast<float>(node.rotation[1]),
	      static_cast<float>(node.rotation[2]));
	m *= glm::mat4_cast(q);

	m = glm::scale(
	    m,
	    glm::vec3(
	        static_cast<float>(node.scale[0]),
	        static_cast<float>(node.scale[1]),
	        static_cast<float>(node.scale[2])
	    )
	);
	return m;
}

TextureHandle LoadTextureFromGltf(
    const std::shared_ptr<IRenderer>& r,
    const tg3_model& model,
    const std::filesystem::path& baseDir,
    std::size_t imageIndex,
    std::unordered_map<std::size_t, TextureHandle>& cache
) {
	if (auto it = cache.find(imageIndex); it != cache.end()) {
		return it->second;
	}

	if (!IsValidIndexU(static_cast<uint32_t>(imageIndex), model.images_count)) {
		cache[imageIndex] = {};
		return {};
	}

	const tg3_image& img = model.images[imageIndex];

	if (img.uri.len == 0 || tg3_is_data_uri(img.uri.data, img.uri.len)) {
		Log::Warning(LogCat::FBXLoader, "Image {} has no local file URI; skipping.", imageIndex);
		cache[imageIndex] = {};
		return {};
	}

	const std::filesystem::path filePath = baseDir / std::filesystem::path(ToStr(img.uri));

	std::error_code ec;
	if (!std::filesystem::exists(filePath, ec) || ec) {
		Log::Warning(LogCat::FBXLoader, "Texture file not found: {}", filePath.string());
		cache[imageIndex] = {};
		return {};
	}

	Texture<glm::vec4> tex = TextureLoader::LoadTextureFloatRGBA(filePath);
	TextureHandle h{};
	if (tex.GetPixelsCount() > 0) {
		h = r->CreateTexture(&tex.GetImage());
	} else {
		Log::Warning(LogCat::FBXLoader, "Failed to decode texture: {}", filePath.string());
	}

	cache[imageIndex] = h;
	return h;
}

std::shared_ptr<PBRMaterial> BuildMaterial(
    const std::shared_ptr<IRenderer>& r,
    const tg3_model& model,
    const std::filesystem::path& baseDir,
    const tg3_material& gm,
    std::unordered_map<std::size_t, TextureHandle>& texCache,
    const FallbackTextures& fallbackTex
) {
	auto mat = std::make_shared<PBRMaterial>();

	const auto& pbr = gm.pbr_metallic_roughness;
	mat->SetAlbedo(glm::vec3(
	    static_cast<float>(pbr.base_color_factor[0]),
	    static_cast<float>(pbr.base_color_factor[1]),
	    static_cast<float>(pbr.base_color_factor[2])
	));
	mat->SetMetallic(static_cast<float>(pbr.metallic_factor));
	mat->SetRoughness(static_cast<float>(pbr.roughness_factor));

	auto resolveTex = [&](int32_t texIdx) -> TextureHandle {
		if (!IsValidIndex(texIdx, model.textures_count)) {
			return {};
		}
		const tg3_texture& tex = model.textures[texIdx];
		if (!IsValidIndex(tex.source, model.images_count)) {
			return {};
		}
		return LoadTextureFromGltf(r, model, baseDir, static_cast<std::size_t>(tex.source), texCache);
	};

	TextureHandle albedo = (pbr.base_color_texture.index >= 0) ? resolveTex(pbr.base_color_texture.index)
	                                                           : TextureHandle{};
	mat->SetAlbedoTexture(ResolveOrFallback(albedo, fallbackTex.albedo));

	TextureHandle mr = (pbr.metallic_roughness_texture.index >= 0) ? resolveTex(pbr.metallic_roughness_texture.index)
	                                                               : TextureHandle{};
	mat->SetMetallicTexture(ResolveOrFallback(mr, fallbackTex.metallic));
	mat->SetRoughnessTexture(ResolveOrFallback(mr, fallbackTex.roughness));

	TextureHandle normal = (gm.normal_texture.index >= 0) ? resolveTex(gm.normal_texture.index) : TextureHandle{};
	mat->SetNormalTexture(ResolveOrFallback(normal, fallbackTex.normal));

	return mat;
}

int32_t FindAttribute(const tg3_primitive& prim, const char* name) {
	for (uint32_t i = 0; i < prim.attributes_count; ++i) {
		if (tg3_str_equals_cstr(prim.attributes[i].key, name)) {
			return prim.attributes[i].value;
		}
	}
	return -1;
}

bool LoadPrimitiveToMesh(const tg3_model& model, const tg3_primitive& prim, Mesh& out) {
	const int32_t posIdx = FindAttribute(prim, "POSITION");
	if (!IsValidIndex(posIdx, model.accessors_count)) {
		Log::Warning(LogCat::FBXLoader, "Primitive without POSITION attribute; skipping.");
		return false;
	}

	const tg3_accessor& posAcc = model.accessors[posIdx];
	const std::size_t vertexCount = static_cast<std::size_t>(posAcc.count);

	std::vector<glm::vec3> positions;
	if (!ReadVecAttribute<float, 3>(model, posAcc, positions)) {
		return false;
	}

	std::vector<glm::vec3> normals(vertexCount, glm::vec3(0.0f));
	const int32_t nrmIdx = FindAttribute(prim, "NORMAL");
	if (IsValidIndex(nrmIdx, model.accessors_count)) {
		ReadVecAttribute<float, 3>(model, model.accessors[nrmIdx], normals);
	}

	std::vector<glm::vec2> uvs(vertexCount, glm::vec2(0.0f));
	const int32_t uvIdx = FindAttribute(prim, "TEXCOORD_0");
	if (IsValidIndex(uvIdx, model.accessors_count)) {
		ReadVecAttribute<float, 2>(model, model.accessors[uvIdx], uvs);
	}

	out.vertexes.resize(vertexCount);
	for (std::size_t i = 0; i < vertexCount; ++i) {
		Vertex v{};
		v.position = positions[i];
		v.normal = (glm::length(normals[i]) > 1e-8f) ? glm::normalize(normals[i]) : glm::vec3(0.0f, 1.0f, 0.0f);
		v.uv = uvs[i];
		out.vertexes[i] = v;
	}

	if (IsValidIndex(prim.indices, model.accessors_count)) {
		const tg3_accessor& idxAcc = model.accessors[prim.indices];
		out.indexes.resize(static_cast<std::size_t>(idxAcc.count));

		switch (idxAcc.component_type) {
		case TG3_COMPONENT_TYPE_UNSIGNED_INT:
			if (!ReadScalarIndices<std::uint32_t>(model, idxAcc, out.indexes)) {
				return false;
			}
			break;
		case TG3_COMPONENT_TYPE_UNSIGNED_SHORT:
			if (!ReadScalarIndices<std::uint16_t>(model, idxAcc, out.indexes)) {
				return false;
			}
			break;
		case TG3_COMPONENT_TYPE_UNSIGNED_BYTE:
			if (!ReadScalarIndices<std::uint8_t>(model, idxAcc, out.indexes)) {
				return false;
			}
			break;
		default:
			Log::Warning(LogCat::FBXLoader, "Unsupported index component type: {}", idxAcc.component_type);
			return false;
		}
	} else {
		out.indexes.resize(vertexCount);
		for (std::size_t i = 0; i < vertexCount; ++i) {
			out.indexes[i] = static_cast<std::int32_t>(i);
		}
	}

	return true;
}

} // namespace

std::shared_ptr<Scene> SceneLoader::LoadGLTF(const std::filesystem::path& path, std::shared_ptr<IRenderer> r) {
	std::shared_ptr<Scene> scene = std::make_shared<Scene>(path.stem().string());

	tg3_parse_options options;
	tg3_parse_options_init(&options);

	tg3_error_stack errors;
	tg3_error_stack_init(&errors);

	tg3_model model;
	std::memset(&model, 0, sizeof(model));
	model.default_scene = -1;

	const std::string pathStr = path.string();
	const tg3_error_code
	    err = tg3_parse_file(&model, &errors, pathStr.c_str(), static_cast<uint32_t>(pathStr.size()), &options);

	if (err != TG3_OK || tg3_errors_has_error(&errors)) {
		const uint32_t count = tg3_errors_count(&errors);
		for (uint32_t i = 0; i < count; ++i) {
			const tg3_error_entry* e = tg3_errors_get(&errors, i);
			if (!e) {
				continue;
			}
			Log::Error(
			    LogCat::FBXLoader,
			    "tinygltf v3 [{}:{}]: {}",
			    static_cast<int>(e->severity),
			    static_cast<int>(e->code),
			    e->message ? e->message : "(no message)"
			);
		}
		tg3_error_stack_free(&errors);
		tg3_model_free(&model);
		return scene;
	}

	Log::Info(
	    LogCat::FBXLoader,
	    "Opened {}: {} meshes, {} materials, {} nodes",
	    pathStr,
	    static_cast<unsigned>(model.meshes_count),
	    static_cast<unsigned>(model.materials_count),
	    static_cast<unsigned>(model.nodes_count)
	);

	const std::filesystem::path baseDir = path.parent_path();

	std::unordered_map<std::size_t, TextureHandle> textureCache;
	std::unordered_map<std::size_t, MaterialHandle> materialHandleCache;
	std::unordered_map<std::size_t, std::shared_ptr<PBRMaterial>> materialPtrCache;

	const FallbackTextures fallbackTex = CreateFallbackTextures(r);

	auto fallback = std::make_shared<PBRMaterial>();
	fallback->SetAlbedo({ 1.0f, 1.0f, 1.0f });
	fallback->SetMetallic(0.0f);
	fallback->SetRoughness(0.8f);
	fallback->SetAlbedoTexture(fallbackTex.albedo);
	fallback->SetMetallicTexture(fallbackTex.metallic);
	fallback->SetRoughnessTexture(fallbackTex.roughness);
	fallback->SetNormalTexture(fallbackTex.normal);
	MaterialHandle fallbackHandle = r->CreateMaterial(fallback.get());
	fallback->SetHandle(fallbackHandle);

	for (std::size_t mi = 0; mi < model.materials_count; ++mi) {
		auto mat = BuildMaterial(r, model, baseDir, model.materials[mi], textureCache, fallbackTex);
		MaterialHandle mh = r->CreateMaterial(mat.get());
		mat->SetHandle(mh);

		materialPtrCache[mi] = mat;
		materialHandleCache[mi] = mh;
	}

	struct LoadedPrimitive {
		std::shared_ptr<Mesh> mesh;
		MeshHandle handle;
		std::size_t materialIndex = kNoIndex;
	};

	std::vector<std::vector<LoadedPrimitive>> meshPrimitives(model.meshes_count);

	for (std::size_t mi = 0; mi < model.meshes_count; ++mi) {
		const tg3_mesh& mesh = model.meshes[mi];
		meshPrimitives[mi].resize(mesh.primitives_count);

		for (std::size_t pi = 0; pi < mesh.primitives_count; ++pi) {
			const tg3_primitive& prim = mesh.primitives[pi];

			auto meshData = std::make_shared<Mesh>();
			if (!LoadPrimitiveToMesh(model, prim, *meshData)) {
				continue;
			}

			MeshHandle mh = r->CreateMesh(meshData.get());

			LoadedPrimitive lp;
			lp.mesh = std::move(meshData);
			lp.handle = mh;
			lp.materialIndex = IsValidIndex(prim.material, model.materials_count)
			                       ? static_cast<std::size_t>(prim.material)
			                       : kNoIndex;
			meshPrimitives[mi][pi] = std::move(lp);
		}
	}

	std::size_t entityCount = 0;

	std::function<void(int32_t, const glm::mat4&, Scene::Entity)> processNode =
	    [&](int32_t nodeIndex, const glm::mat4& parentWorld, Scene::Entity parentEntity) {
		    const tg3_node& node = model.nodes[nodeIndex];
		    const glm::mat4 world = parentWorld * NodeTransform(node);

			Scene::Entity selfEntity = parentEntity;

		    if (IsValidIndex(node.mesh, model.meshes_count)) {
			    const std::size_t meshIdx = static_cast<std::size_t>(node.mesh);
			    const auto& prims = meshPrimitives[meshIdx];
			    const std::string nodeName = node.name.len > 0 ? ToStr(node.name) : std::format("Mesh_{}", meshIdx);

			    for (std::size_t pi = 0; pi < prims.size(); ++pi) {
				    const auto& lp = prims[pi];
				    if (!lp.mesh || !lp.handle) {
					    continue;
				    }

				    std::shared_ptr<IMaterial> matPtr = fallback;
				    MaterialHandle matHandle = fallbackHandle;
				    if (lp.materialIndex != kNoIndex) {
					    if (auto it = materialPtrCache.find(lp.materialIndex); it != materialPtrCache.end()) {
						    matPtr = it->second;
					    }
					    if (auto it = materialHandleCache.find(lp.materialIndex); it != materialHandleCache.end()) {
						    matHandle = it->second;
					    }
				    }

				    const std::string name = (prims.size() == 1) ? nodeName : std::format("{}_{}", nodeName, pi);

				    Scene::Entity e = scene->CreateEntity(name, parentEntity);

				    scene->AddComponent<MeshComponent>(e, MeshComponent{ lp.mesh, lp.handle });
				    scene->AddComponent<MaterialComponent>(e, MaterialComponent{ matPtr, matHandle });
				    scene->AddComponent<WorldMatrixComponent>(e, WorldMatrixComponent{ world });

				    ++entityCount;
			    }
		    }

		    for (uint32_t ci = 0; ci < node.children_count; ++ci) {
			    const int32_t childIdx = node.children[ci];
			    if (IsValidIndex(childIdx, model.nodes_count)) {
				    processNode(childIdx, world, selfEntity);
			    }
		    }
	    };

	if (IsValidIndex(model.default_scene, model.scenes_count)) {
		const tg3_scene& ds = model.scenes[model.default_scene];
		for (uint32_t i = 0; i < ds.nodes_count; ++i) {
			if (IsValidIndex(ds.nodes[i], model.nodes_count)) {
				processNode(ds.nodes[i], glm::mat4(1.0f), scene->GetRoot());
			}
		}
	} else {
		std::vector<bool> isChild(model.nodes_count, false);
		for (uint32_t i = 0; i < model.nodes_count; ++i) {
			const tg3_node& n = model.nodes[i];
			for (uint32_t ci = 0; ci < n.children_count; ++ci) {
				const int32_t c = n.children[ci];
				if (IsValidIndex(c, model.nodes_count)) {
					isChild[static_cast<std::size_t>(c)] = true;
				}
			}
		}
		for (uint32_t i = 0; i < model.nodes_count; ++i) {
			if (!isChild[i]) {
				processNode(static_cast<int32_t>(i), glm::mat4(1.0f), scene->GetRoot());
			}
		}
	}

	Log::Info(
	    LogCat::FBXLoader,
	    "Loaded {} mesh entities, {} materials, {} textures",
	    entityCount,
	    static_cast<unsigned>(model.materials_count),
	    static_cast<unsigned>(textureCache.size())
	);

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

	tg3_error_stack_free(&errors);
	tg3_model_free(&model);

	return scene;
}

} // namespace PixieToolbox
