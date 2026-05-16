#pragma once
#include <string>

#include "Mesh/Mesh.h"
#include "Rendering/TextureEnums.h"
#include "Rendering/ResourceHandles.h"

class RendererBase {
public:
	virtual void StartFrame() = 0;
	virtual void EndFrame() = 0;

	virtual void DrawMesh(MeshHandle meshHandle, ShaderHandle shaderHandle) = 0;
	virtual MeshHandle LoadMesh(Mesh* mesh) = 0;
	virtual void LoadMesh(MeshHandle& handle, Mesh* mesh) = 0;

	virtual FrameBufferHandle CreateFrameBuffer(glm::ivec2 resolution) = 0;
	virtual void ResizeFrameBuffer(FrameBufferHandle& handle, glm::ivec2 resolution) = 0;
	virtual void BindFrameBuffer(FrameBufferHandle handle) = 0;
	virtual void UnbindFrameBuffer() = 0;
	virtual void ClearFrameBuffer() = 0;

	virtual void ResizeViewport(glm::ivec2 resolution) = 0;

	virtual TextureHandle CreateTexture(glm::ivec2 resolution, TextureFormat format) = 0;
	virtual TextureHandle LoadTexture(float* data, glm::ivec2 resolution) = 0;
	virtual TextureHandle LoadTexture(glm::vec3* data, glm::ivec2 resolution) = 0;
	virtual TextureHandle LoadTexture(glm::vec4* data, glm::ivec2 resolution) = 0;
	virtual void LoadTexture(TextureHandle handle, float* data, glm::ivec2 resolution) = 0;
	virtual void LoadTexture(TextureHandle handle, glm::vec3* data, glm::ivec2 resolution) = 0;
	virtual void LoadTexture(TextureHandle handle, glm::vec4* data, glm::ivec2 resolution) = 0;
	virtual void SetTextureFiltering(const TextureHandle handle, TextureFiltering minFilter, TextureFiltering magFilter) = 0;
	virtual void SetTextureWrap(const TextureHandle handle, TextureWrap wrapS, TextureWrap wrapT) = 0;
	virtual void GenerateTextureMipmaps(const TextureHandle handle) = 0;
	virtual glm::ivec2 GetTextureResolution(TextureHandle handle) = 0;

	virtual ShaderStorageBufferHandle LoadShaderStorageBuffer(uint8_t* data, uint32_t size) = 0;
	virtual void LoadShaderStorageBuffer(ShaderStorageBufferHandle handle, uint8_t* data, uint32_t size) = 0;
	virtual void BindShaderStorageBuffer(ShaderStorageBufferHandle handle, uint32_t index) = 0;
	virtual uint32_t GetShaderStorageBufferSize(ShaderStorageBufferHandle handle) = 0;
	virtual std::vector<uint8_t> GetShaderStorageBufferData(ShaderStorageBufferHandle handle, uint32_t offset, uint32_t size) = 0;

	virtual ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string fragmentShaderShource) = 0;
	virtual void SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value) = 0;
	virtual void SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value) = 0;
	virtual void SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v) = 0;
	virtual void SetUniform1iv(ShaderHandle handle, const std::string& name, int32_t* data, int32_t count) = 0;
	virtual void SetUniform1f(ShaderHandle handle, const std::string& name, float value) = 0;
	virtual void SetUniform1fv(ShaderHandle handle, const std::string& name, float* data, int32_t count) = 0;
	virtual void SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v) = 0;
	virtual void SetUniform2fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v) = 0;
	virtual void SetUniform3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v) = 0;
	virtual void SetUniform4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false) = 0;
	virtual void SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false) = 0;
	virtual void SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false) = 0;
	virtual void SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false) = 0;
	virtual void BindTexture(ShaderHandle shaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index) = 0;

	virtual ComputeShaderHandle CreateComputeShader(const std::string& source) = 0;
	virtual void DispatchComputeShader(ComputeShaderHandle handle, int32_t x, int32_t y, int32_t z) = 0;
	virtual void SetUniform1i(ComputeShaderHandle handle, const std::string& name, int32_t value) = 0;
	virtual void SetUniform1ui(ComputeShaderHandle handle, const std::string& name, uint32_t value) = 0;
	virtual void SetUniform2i(ComputeShaderHandle handle, const std::string& name, glm::ivec2 v) = 0;
	virtual void SetUniform1iv(ComputeShaderHandle handle, const std::string& name, int32_t* data, int32_t count) = 0;
	virtual void SetUniform1f(ComputeShaderHandle handle, const std::string& name, float value) = 0;
	virtual void SetUniform1fv(ComputeShaderHandle handle, const std::string& name, float* data, int32_t count) = 0;
	virtual void SetUniform2f(ComputeShaderHandle handle, const std::string& name, glm::vec2 v) = 0;
	virtual void SetUniform2fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniform3f(ComputeShaderHandle handle, const std::string& name, glm::vec3 v) = 0;
	virtual void SetUniform3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniform4f(ComputeShaderHandle handle, const std::string& name, glm::vec4 v) = 0;
	virtual void SetUniform4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) = 0;
	virtual void SetUniformMat3f(ComputeShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false) = 0;
	virtual void SetUniformMat3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false) = 0;
	virtual void SetUniformMat4f(ComputeShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false) = 0;
	virtual void SetUniformMat4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false) = 0;
	virtual void BindTexture(ComputeShaderHandle computeShaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index) = 0;

	virtual void MemoryBarriersAll() = 0;

	virtual uint64_t GetInternalID(TextureHandle handle) = 0;
	virtual uint64_t GetInternalColorAttachmentID(FrameBufferHandle handle) = 0;
	virtual uint64_t GetInternalDepthAttachmentID(FrameBufferHandle handle) = 0;
};
