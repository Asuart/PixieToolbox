#include "SceneLoader.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include <glm/glm.hpp>

#include <pxr/base/gf/matrix4d.h>
#include <pxr/base/vt/array.h>
#include <pxr/usd/sdf/assetPath.h>
#include <pxr/usd/usd/primRange.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdGeom/mesh.h>
#include <pxr/usd/usdGeom/primvarsAPI.h>
#include <pxr/usd/usdGeom/xformable.h>
#include <pxr/usd/usdShade/connectableAPI.h>
#include <pxr/usd/usdShade/material.h>
#include <pxr/usd/usdShade/materialBindingAPI.h>
#include <pxr/usd/usdShade/shader.h>

using namespace PixieRenderer;

static std::string TfToString(const pxr::TfToken& t) {
	return t.GetString();
}

static glm::mat4 GfMatrixToGlm(const pxr::GfMatrix4d& m) {
	glm::mat4 r(1.0f);
	for (int row = 0; row < 4; row++)
		for (int col = 0; col < 4; col++)
			r[col][row] = static_cast<float>(m[row][col]);
	return r;
}

static std::string ResolveTexturePath(const std::string& filename) {
	if (filename.empty())
		return filename;
	std::filesystem::path p(filename);
	std::error_code ec;
	if (std::filesystem::exists(p, ec))
		return p.string();
	auto local = gAssetRoot / p.filename();
	if (std::filesystem::exists(local, ec))
		return local.string();
	auto rel = gAssetRoot / p;
	if (std::filesystem::exists(rel, ec))
		return rel.string();
	return filename;
}

static TextureHandle GetTextureFromInput(
    const pxr::UsdShadeInput& input,
    SceneData& s,
    IRenderer* r,
    glm::vec4 fallback,
    TextureFormat fmt,
    bool srgb
) {
	if (!input || !input.HasConnectedSource())
		return CreateSolidTexture(r, fallback, fmt);

	pxr::UsdShadeConnectableAPI source;
	pxr::TfToken sourceName;
	pxr::UsdShadeAttributeType sourceType;
	if (!input.GetConnectedSource(&source, &sourceName, &sourceType))
		return CreateSolidTexture(r, fallback, fmt);

	if (!source.GetPrim().IsA<pxr::UsdShadeShader>())
		return CreateSolidTexture(r, fallback, fmt);

	pxr::UsdShadeShader shader(source.GetPrim());
	auto fileInput = shader.GetInput(pxr::TfToken("file"));
	if (!fileInput)
		return CreateSolidTexture(r, fallback, fmt);

	pxr::SdfAssetPath assetPath;
	if (!fileInput.Get(&assetPath))
		return CreateSolidTexture(r, fallback, fmt);

	std::string texPath = assetPath.GetResolvedPath();
	if (texPath.empty())
		texPath = assetPath.GetAssetPath();
	if (texPath.empty())
		return CreateSolidTexture(r, fallback, fmt);

	return LoadTextureCached(s, r, ResolveTexturePath(texPath), srgb);
}

static TextureHandle CreateSolidTexture(IRenderer* r, glm::vec4 v, TextureFormat fmt) {
	Image2D img;
	img.resolution = { 1, 1 };
	img.format = fmt;

	auto toByte = [](float x) { return static_cast<uint8_t>(std::clamp(x * 255.0f + 0.5f, 0.0f, 255.0f)); };

	switch (fmt) {
	case TextureFormat::RGBA8:
		img.pixels.resize(4);
		img.pixels[0] = toByte(v.x);
		img.pixels[1] = toByte(v.y);
		img.pixels[2] = toByte(v.z);
		img.pixels[3] = toByte(v.w);
		break;
	case TextureFormat::Red8:
		img.pixels.resize(1);
		img.pixels[0] = toByte(v.x);
		break;
	case TextureFormat::RGBA32f:
		img.pixels.resize(sizeof(glm::vec4));
		*reinterpret_cast<glm::vec4*>(img.pixels.data()) = v;
		break;
	case TextureFormat::Red32f:
		img.pixels.resize(sizeof(float));
		*reinterpret_cast<float*>(img.pixels.data()) = v.x;
		break;
	default:
		img.pixels.resize(4);
		img.pixels[0] = toByte(v.x);
		img.pixels[1] = toByte(v.y);
		img.pixels[2] = toByte(v.z);
		img.pixels[3] = toByte(v.w);
		break;
	}
	return r->CreateTexture(&img);
}

namespace PixieToolbox {

std::unique_ptr<Scene> SceneLoader::LoadScene(std::filesystem::path path) {
	pxr::UsdStageRefPtr stage = pxr::UsdStage::Open(path);
	if (!stage) {
		std::cerr << "Failed to open USD stage: " << path << "\n";
		std::exit(1);
	}

	const pxr::UsdTimeCode time = pxr::UsdTimeCode::Default();

	std::cout << "USD stage opened: " << path << "\n";

	// ------------------------------------------------------------------
	// Pass 1: материалы
	// ------------------------------------------------------------------
	s.materials.reserve(64);
	s.materialHandles.reserve(64);

	std::unordered_map<std::string, size_t> matIndex;

	for (const auto& prim : stage->Traverse()) {
		if (!prim.IsA<pxr::UsdShadeMaterial>())
			continue;

		std::string matKey = prim.GetPath().GetString();
		if (matIndex.count(matKey))
			continue;

		pxr::UsdShadeMaterial usdMat(prim);
		auto mat = std::make_unique<PBRMaterial>();

		// UsdPreviewSurface: значения по умолчанию
		glm::vec3 albedo(1.0f);
		float metallic = 0.0f;
		float roughness = 0.5f;

		pxr::UsdShadeShader surface = usdMat.ComputeSurfaceSource();

		if (surface) {
			auto diffuseInput = surface.GetInput(pxr::TfToken("diffuseColor"));
			auto metallicInput = surface.GetInput(pxr::TfToken("metallic"));
			auto roughInput = surface.GetInput(pxr::TfToken("roughness"));
			auto normalInput = surface.GetInput(pxr::TfToken("normal"));

			pxr::GfVec3f diffuseVal(1.0f);
			if (diffuseInput && diffuseInput.Get(&diffuseVal))
				albedo = { diffuseVal[0], diffuseVal[1], diffuseVal[2] };

			if (metallicInput && metallicInput.Get(&metallic)) {
			}
			if (roughInput && roughInput.Get(&roughness)) {
			}

			mat->SetAlbedoTexture(GetTextureFromInput(diffuseInput, s, r, { 1, 1, 1, 1 }, TextureFormat::RGBA32f, true)
			);
			mat->SetNormalTexture(
			    GetTextureFromInput(normalInput, s, r, { 0.5f, 0.5f, 1, 1 }, TextureFormat::RGBA32f, false)
			);
			mat->SetMetallicTexture(
			    GetTextureFromInput(metallicInput, s, r, { 0, 0, 0, 1 }, TextureFormat::Red32f, false)
			);
			mat->SetRoughnessTexture(GetTextureFromInput(roughInput, s, r, { 1, 1, 1, 1 }, TextureFormat::Red32f, false)
			);
		} else {
			//mat->SetAlbedoTexture(CreateSolidTexture(r, { 1, 1, 1, 1 }, TextureFormat::RGBA32f));
			//mat->SetNormalTexture(CreateSolidTexture(r, { 0.5f, 0.5f, 1, 1 }, TextureFormat::RGBA32f));
			//mat->SetMetallicTexture(CreateSolidTexture(r, { 0, 0, 0, 1 }, TextureFormat::Red32f));
			//mat->SetRoughnessTexture(CreateSolidTexture(r, { 1, 1, 1, 1 }, TextureFormat::Red32f));
		}

		mat->SetAlbedo(albedo);
		mat->SetMetallic(metallic);
		mat->SetRoughness(roughness);

		MaterialHandle mh = r->CreateMaterial(mat.get());
		mat->SetHandle(mh);

		matIndex[matKey] = s.materialHandles.size();
		s.materialHandles.push_back(mh);
		s.materials.push_back(std::move(mat));
	}

	if (s.materialHandles.empty()) {
		auto mat = std::make_unique<PBRMaterial>();
		mat->SetAlbedo({ 1, 1, 1 });
		mat->SetMetallic(0.0f);
		mat->SetRoughness(0.8f);
		//mat->SetAlbedoTexture(CreateSolidTexture(r, { 1, 1, 1, 1 }, TextureFormat::RGBA32f));
		//mat->SetNormalTexture(CreateSolidTexture(r, { 0.5f, 0.5f, 1, 1 }, TextureFormat::RGBA32f));
		//mat->SetMetallicTexture(CreateSolidTexture(r, { 0, 0, 0, 1 }, TextureFormat::Red32f));
		//mat->SetRoughnessTexture(CreateSolidTexture(r, { 1, 1, 1, 1 }, TextureFormat::Red32f));

		MaterialHandle mh = r->CreateMaterial(mat.get());
		mat->SetHandle(mh);
		s.materialHandles.push_back(mh);
		s.materials.push_back(std::move(mat));
		std::cout << "Scene has no materials — using default PBR material.\n";
	} else {
		std::cout << "Loaded " << s.materialHandles.size() << " material(s).\n";
	}

	// ------------------------------------------------------------------
	// Pass 2: меши
	// ------------------------------------------------------------------
	size_t meshCount = 0;

	for (const auto& prim : stage->Traverse()) {
		if (!prim.IsA<pxr::UsdGeomMesh>())
			continue;
		if (prim.IsPrototype())
			continue;
		if (!prim.IsActive())
			continue;
		if (prim.IsInstance())
			continue; // инстансы пока не разворачиваем

		pxr::UsdGeomMesh usdMesh(prim);

		// Пропускаем невидимые
		auto vis = usdMesh.ComputeVisibility(time);
		if (vis == pxr::UsdGeomTokens->invisible)
			continue;

		// --- Геометрия ---
		pxr::VtArray<pxr::GfVec3f> points;
		if (!usdMesh.GetPointsAttr().Get(&points, time) || points.empty())
			continue;

		pxr::VtArray<pxr::GfVec3f> normals;
		usdMesh.GetNormalsAttr().Get(&normals, time);
		pxr::TfToken normalsInterp = usdMesh.GetNormalsInterpolation();

		// UV: примарвар "st" (стандарт USD). Иногда "UVMap" или "uv".
		pxr::VtArray<pxr::GfVec2f> uvs;
		pxr::TfToken uvInterp;
		{
			pxr::UsdGeomPrimvarsAPI primvars(prim);
			auto stPv = primvars.GetPrimvar(pxr::TfToken("st"));
			if (!stPv)
				stPv = primvars.GetPrimvar(pxr::TfToken("UVMap"));
			if (!stPv)
				stPv = primvars.GetPrimvar(pxr::TfToken("uv"));
			if (stPv) {
				stPv.Get(&uvs, time);
				uvInterp = stPv.GetInterpolation();
			}
		}

		pxr::VtArray<int> faceVertexCounts;
		pxr::VtArray<int> faceVertexIndices;
		usdMesh.GetFaceVertexCountsAttr().Get(&faceVertexCounts, time);
		usdMesh.GetFaceVertexIndicesAttr().Get(&faceVertexIndices, time);
		if (faceVertexCounts.empty() || faceVertexIndices.empty())
			continue;

		// --- Мировой трансформ ---
		pxr::UsdGeomXformable xformable(prim);
		pxr::GfMatrix4d worldXform = xformable.ComputeLocalToWorldTransform(time);
		glm::mat4 world = GfMatrixToGlm(worldXform);

		// --- Материал ---
		pxr::UsdShadeMaterialBindingAPI bindingAPI(prim);
		pxr::UsdShadeMaterial boundMat = bindingAPI.ComputeBoundMaterial();
		std::string matKey = boundMat ? boundMat.GetPath().GetString() : "";

		size_t safeId = 0;
		auto it = matIndex.find(matKey);
		if (it != matIndex.end())
			safeId = it->second;

		// --- Триангуляция (fan) ---
		Mesh meshData;
		size_t indexOffset = 0;

		for (int fi = 0; fi < static_cast<int>(faceVertexCounts.size()); fi++) {
			const int count = faceVertexCounts[fi];
			if (count < 3) {
				indexOffset += count;
				continue;
			}

			for (int i = 2; i < count; i++) {
				const int tri[3] = { faceVertexIndices[indexOffset],
					                 faceVertexIndices[indexOffset + i - 1],
					                 faceVertexIndices[indexOffset + i] };

				// Для faceVarying-примарваров индекс в массиве совпадает
				// с индексом в faceVertexIndices.
				const int fvIdx[3] = { static_cast<int>(indexOffset),
					                   static_cast<int>(indexOffset + i - 1),
					                   static_cast<int>(indexOffset + i) };

				for (int j = 0; j < 3; j++) {
					const int vi = tri[j];
					Vertex v{};

					v.position = { points[vi][0], points[vi][1], points[vi][2] };

					// Нормали
					if (!normals.empty()) {
						int ni = (normalsInterp == pxr::UsdGeomTokens->faceVarying) ? fvIdx[j] : vi;
						if (ni >= 0 && ni < static_cast<int>(normals.size()))
							v.normal = { normals[ni][0], normals[ni][1], normals[ni][2] };
					}
					if (glm::length(v.normal) < 1e-8f)
						v.normal = glm::vec3(0, 1, 0);
					else
						v.normal = glm::normalize(v.normal);

					// UV
					if (!uvs.empty()) {
						int ui = (uvInterp == pxr::UsdGeomTokens->faceVarying) ? fvIdx[j] : vi;
						if (ui >= 0 && ui < static_cast<int>(uvs.size()))
							v.uv = { uvs[ui][0], uvs[ui][1] };
					}

					meshData.vertexes.push_back(v);
					meshData.indexes.push_back(static_cast<int32_t>(meshData.vertexes.size() - 1));
				}
			}
			indexOffset += count;
		}

		if (meshData.vertexes.empty())
			continue;

		MeshHandle mh = r->CreateMesh(&meshData);
		s.meshStorage.push_back(mh);
		s.items.push_back({ mh, s.materialHandles[safeId], world });
		meshCount++;
	}

	std::cout << "Loaded: " << s.items.size() << " draw items, " << s.materialHandles.size() << " material handle(s), "
	          << s.textureCache.size() << " cached texture(s)\n";
}

} // namespace PixieToolbox
