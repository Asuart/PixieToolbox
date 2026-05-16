#pragma once
#include <glad/glad.h>
#include <vector>

#include "Rendering/RendererBase.h"
#include "MeshOpenGL.h"
#include "TextureOpenGL.h"
#include "FrameBufferOpenGL.h"
#include "ShaderCompilationOpenGL.h"
#include "ViewportStateOpenGL.h"
#include "ShaderStorageBufferOpenGL.h"

class RendererOpenGL : public RendererBase {
public:
	RendererOpenGL();
	~RendererOpenGL();

	void StartFrame();
	void EndFrame();

	void DrawMesh(MeshHandle meshHandle, ShaderHandle shaderHandle);
	MeshHandle LoadMesh(Mesh* mesh);
	void LoadMesh(MeshHandle& handle, Mesh* mesh);

	FrameBufferHandle CreateFrameBuffer(glm::ivec2 resolution);
	void ResizeFrameBuffer(FrameBufferHandle& handle, glm::ivec2 resolution);
	void BindFrameBuffer(FrameBufferHandle handle);
	void UnbindFrameBuffer();
	void ClearFrameBuffer();

	void ResizeViewport(glm::ivec2 resolution);

	TextureHandle CreateTexture(glm::ivec2 resolution, TextureFormat format);
	TextureHandle LoadTexture(float* data, glm::ivec2 resolution);
	TextureHandle LoadTexture(glm::vec3* data, glm::ivec2 resolution);
	TextureHandle LoadTexture(glm::vec4* data, glm::ivec2 resolution);
	void LoadTexture(TextureHandle handle, float* data, glm::ivec2 resolution);
	void LoadTexture(TextureHandle handle, glm::vec3* data, glm::ivec2 resolution);
	void LoadTexture(TextureHandle handle, glm::vec4* data, glm::ivec2 resolution);
	void SetTextureFiltering(const TextureHandle handle, TextureFiltering minFilter, TextureFiltering magFilter);
	void SetTextureWrap(const TextureHandle handle, TextureWrap wrapS, TextureWrap wrapT);
	void GenerateTextureMipmaps(const TextureHandle handle);
	glm::ivec2 GetTextureResolution(TextureHandle handle);

	ShaderStorageBufferHandle LoadShaderStorageBuffer(uint8_t* data, uint32_t size);
	void LoadShaderStorageBuffer(ShaderStorageBufferHandle handle, uint8_t* data, uint32_t size);
	void BindShaderStorageBuffer(ShaderStorageBufferHandle handle, uint32_t index);
	uint32_t GetShaderStorageBufferSize(ShaderStorageBufferHandle handle);
	std::vector<uint8_t> GetShaderStorageBufferData(ShaderStorageBufferHandle handle, uint32_t offset, uint32_t size);

	ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource);
	void SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value);
	void SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value);
	void SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v);
	void SetUniform1iv(ShaderHandle handle, const std::string& name, int32_t* data, int32_t count);
	void SetUniform1f(ShaderHandle handle, const std::string& name, float value);
	void SetUniform1fv(ShaderHandle handle, const std::string& name, float* data, int32_t count);
	void SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v);
	void SetUniform2fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v);
	void SetUniform3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v);
	void SetUniform4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false);
	void SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	void SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false);
	void SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	void BindTexture(ShaderHandle shaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index);

	ComputeShaderHandle CreateComputeShader(const std::string& source);
	void DispatchComputeShader(ComputeShaderHandle handle, int32_t x, int32_t y, int32_t z);
	void SetUniform1i(ComputeShaderHandle handle, const std::string& name, int32_t value);
	void SetUniform1ui(ComputeShaderHandle handle, const std::string& name, uint32_t value);
	void SetUniform2i(ComputeShaderHandle handle, const std::string& name, glm::ivec2 v);
	void SetUniform1iv(ComputeShaderHandle handle, const std::string& name, int32_t* data, int32_t count);
	void SetUniform1f(ComputeShaderHandle handle, const std::string& name, float value);
	void SetUniform1fv(ComputeShaderHandle handle, const std::string& name, float* data, int32_t count);
	void SetUniform2f(ComputeShaderHandle handle, const std::string& name, glm::vec2 v);
	void SetUniform2fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniform3f(ComputeShaderHandle handle, const std::string& name, glm::vec3 v);
	void SetUniform3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniform4f(ComputeShaderHandle handle, const std::string& name, glm::vec4 v);
	void SetUniform4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	void SetUniformMat3f(ComputeShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false);
	void SetUniformMat3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	void SetUniformMat4f(ComputeShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false);
	void SetUniformMat4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	void BindTexture(ComputeShaderHandle computeShaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index);

	void MemoryBarriersAll();

	uint64_t GetInternalID(TextureHandle handle);
	uint64_t GetInternalColorAttachmentID(FrameBufferHandle handle);
	uint64_t GetInternalDepthAttachmentID(FrameBufferHandle handle);

private:
	std::vector<TextureOpenGL> m_textures;
	std::vector<MeshOpenGL> m_meshes;
	std::vector<FrameBufferOpenGL> m_frameBuffers;
	std::vector<ShaderOpenGL> m_shaders;
	std::vector<ComputeShaderOpenGL> m_computeShaders;
	std::vector<ViewportStateOpenGL> m_viewportStates;
	std::vector<ShaderStorageBufferOpenGL> m_shaderStorageBuffers;

	TextureOpenGL& GetTextureEntry(TextureHandle handle);
	MeshOpenGL& GetMeshEntry(MeshHandle handle);
	FrameBufferOpenGL& GetFrameBufferEntry(FrameBufferHandle handle);
	ShaderOpenGL& GetShaderEntry(ShaderHandle handle);
	ComputeShaderOpenGL& GetComputeShaderEntry(ComputeShaderHandle handle);
	ShaderStorageBufferOpenGL& GetShaderStorageBufferEntry(ShaderStorageBufferHandle handle);

	void StoreViewportState();
	void RestoreViewportState();
};
