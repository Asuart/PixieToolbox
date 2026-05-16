#pragma once
#include "RenderAPI.h"
#include "ResourceHandles.h"
#include "RendererBase.h"

class MainWindow;

class RenderEngine {
public:
	static bool Initialize(MainWindow* window, RenderAPI api);
	static void Free();

	static RenderAPI GetRenderAPI();
	static RendererBase* GetRenderer();

	static void StartFrame();
	static void EndFrame();

	static MeshHandle LoadMesh(Mesh* mesh);
	static void LoadMesh(MeshHandle& handle, Mesh* mesh);
	static void DrawMesh(MeshHandle meshHandle, ShaderHandle shaderHandle);

	static FrameBufferHandle CreateFrameBuffer(glm::ivec2 resolution);
	static void ResizeFrameBuffer(FrameBufferHandle& handle, glm::ivec2 resolution);
	static void BindFrameBuffer(FrameBufferHandle handle);
	static void UnbindFrameBuffer();
	static void ClearFrameBuffer();

	static void ResizeViewport(glm::ivec2 resolution);

	static TextureHandle CreateTexture(glm::ivec2 resolution, TextureFormat format);
	static TextureHandle LoadTexture(float* data, glm::ivec2 resolution);
	static TextureHandle LoadTexture(glm::vec3* data, glm::ivec2 resolution);
	static TextureHandle LoadTexture(glm::vec4* data, glm::ivec2 resolution);
	static void LoadTexture(TextureHandle textureHandle, float* data, glm::ivec2 resolution);
	static void LoadTexture(TextureHandle textureHandle, glm::vec3* data, glm::ivec2 resolution);
	static void LoadTexture(TextureHandle textureHandle, glm::vec4* data, glm::ivec2 resolution);
	static void SetTextureFiltering(const TextureHandle handle, TextureFiltering minFilter, TextureFiltering magFilter);
	static void SetTextureWrap(const TextureHandle handle, TextureWrap wrapS, TextureWrap wrapT);
	static void GenerateTextureMipmaps(const TextureHandle handle);
	static glm::ivec2 GetTextureResolution(TextureHandle handle);

	static ShaderStorageBufferHandle LoadShaderStorageBuffer(uint8_t* data, uint32_t size);
	static void LoadShaderStorageBuffer(ShaderStorageBufferHandle handle, uint8_t* data, uint32_t size);
	static void BindShaderStorageBuffer(ShaderStorageBufferHandle handle, uint32_t index);
	static uint32_t GetShaderStorageBufferSize(ShaderStorageBufferHandle handle);
	static std::vector<uint8_t> GetShaderStorageBufferData(ShaderStorageBufferHandle handle, uint32_t offset, uint32_t size);

	static ShaderHandle CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	static void SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value);
	static void SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value);
	static void SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v);
	static void SetUniform1iv(ShaderHandle handle, const std::string& name, int32_t* data, int32_t count);
	static void SetUniform1f(ShaderHandle handle, const std::string& name, float value);
	static void SetUniform1fv(ShaderHandle handle, const std::string& name, float* data, int32_t count);
	static void SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v);
	static void SetUniform2fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v);
	static void SetUniform3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v);
	static void SetUniform4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false);
	static void SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	static void SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false);
	static void SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	static void BindTexture(ShaderHandle computeShaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index);

	static ComputeShaderHandle CreateComputeShader(const std::string& source);
	static void DispatchComputeShader(ComputeShaderHandle handle, int32_t x, int32_t y, int32_t z);
	static void SetUniform1i(ComputeShaderHandle handle, const std::string& name, int32_t value);
	static void SetUniform1ui(ComputeShaderHandle handle, const std::string& name, uint32_t value);
	static void SetUniform2i(ComputeShaderHandle handle, const std::string& name, glm::ivec2 v);
	static void SetUniform1iv(ComputeShaderHandle handle, const std::string& name, int32_t* data, int32_t count);
	static void SetUniform1f(ComputeShaderHandle handle, const std::string& name, float value);
	static void SetUniform1fv(ComputeShaderHandle handle, const std::string& name, float* data, int32_t count);
	static void SetUniform2f(ComputeShaderHandle handle, const std::string& name, glm::vec2 v);
	static void SetUniform2fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniform3f(ComputeShaderHandle handle, const std::string& name, glm::vec3 v);
	static void SetUniform3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniform4f(ComputeShaderHandle handle, const std::string& name, glm::vec4 v);
	static void SetUniform4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count);
	static void SetUniformMat3f(ComputeShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose = false);
	static void SetUniformMat3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	static void SetUniformMat4f(ComputeShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose = false);
	static void SetUniformMat4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose = false);
	static void BindTexture(ComputeShaderHandle computeShaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index);

	static void MemoryBarriersAll();

	static uint64_t GetInternalID(TextureHandle handle);
	static uint64_t GetInternalColorAttachmentID(FrameBufferHandle handle);
	static uint64_t GetInternalDepthAttachmentID(FrameBufferHandle handle);

protected:
	inline static RenderAPI s_renderAPI = RenderAPI::Undefined;
	inline static RendererBase* s_renderer = nullptr;
};
