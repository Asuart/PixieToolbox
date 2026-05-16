#include "RenderEngine.h"
#include "Rendering/OpenGL/RendererOpenGL.h"
#include "Rendering/Vulkan/RendererVulkan.h"
#include "Utils/Log.h"

bool RenderEngine::Initialize(MainWindow* window, RenderAPI api) {
	s_renderAPI = api;
	if (api == RenderAPI::Vulkan) {
		//s_renderer = new RendererVulkan();
		Log::Error("RedndererVulkan is not implemented.");
		return false;
	}
	else if (api == RenderAPI::OpenGL) {
		s_renderer = new RendererOpenGL();
	}
	else {
		Log::Error("Failed to initialize render engine. Unhandled render API.");
		return false;
	}

	return true;
}

void RenderEngine::Free() {
	delete s_renderer;
	s_renderer = nullptr;
}

RenderAPI RenderEngine::GetRenderAPI() {
	return s_renderAPI;
}

RendererBase* RenderEngine::GetRenderer() {
	return s_renderer;
}

void RenderEngine::StartFrame() {
	s_renderer->StartFrame();
}

void RenderEngine::EndFrame() {
	s_renderer->EndFrame();
}

MeshHandle RenderEngine::LoadMesh(Mesh* mesh) {
	return s_renderer->LoadMesh(mesh);
}

void RenderEngine::LoadMesh(MeshHandle& handle, Mesh* mesh) {
	s_renderer->LoadMesh(handle, mesh);
}

void RenderEngine::DrawMesh(MeshHandle meshHandle, ShaderHandle shaderHandle) {
	s_renderer->DrawMesh(meshHandle, shaderHandle);
}

FrameBufferHandle RenderEngine::CreateFrameBuffer(glm::ivec2 resolution){
	return s_renderer->CreateFrameBuffer(resolution);
}

void RenderEngine::ResizeFrameBuffer(FrameBufferHandle& handle, glm::ivec2 resolution){
	s_renderer->ResizeFrameBuffer(handle, resolution);
}

void RenderEngine::BindFrameBuffer(FrameBufferHandle handle) {
	s_renderer->BindFrameBuffer(handle);
}

void RenderEngine::UnbindFrameBuffer() {
	s_renderer->UnbindFrameBuffer();
}

void RenderEngine::ClearFrameBuffer() {
	s_renderer->ClearFrameBuffer();
}

void RenderEngine::ResizeViewport(glm::ivec2 resolution) {
	s_renderer->ResizeViewport(resolution);
}

TextureHandle RenderEngine::CreateTexture(glm::ivec2 resolution, TextureFormat format) {
	return s_renderer->CreateTexture(resolution, format);
}

TextureHandle RenderEngine::LoadTexture(float* data, glm::ivec2 resolution) {
	return s_renderer->LoadTexture(data, resolution);
}

TextureHandle RenderEngine::LoadTexture(glm::vec3* data, glm::ivec2 resolution) {
	return s_renderer->LoadTexture(data, resolution);
}

TextureHandle RenderEngine::LoadTexture(glm::vec4* data, glm::ivec2 resolution) {
	return s_renderer->LoadTexture(data, resolution);
}

void RenderEngine::LoadTexture(TextureHandle textureHandle, float* data, glm::ivec2 resolution) {
	s_renderer->LoadTexture(textureHandle, data, resolution);
}

void RenderEngine::LoadTexture(TextureHandle textureHandle, glm::vec3* data, glm::ivec2 resolution) {
	s_renderer->LoadTexture(textureHandle, data, resolution);
}

void RenderEngine::LoadTexture(TextureHandle textureHandle, glm::vec4* data, glm::ivec2 resolution) {
	s_renderer->LoadTexture(textureHandle, data, resolution);
}

glm::ivec2 RenderEngine::GetTextureResolution(TextureHandle handle) {
	return s_renderer->GetTextureResolution(handle);
}

ShaderStorageBufferHandle RenderEngine::LoadShaderStorageBuffer(uint8_t* data, uint32_t size) {
	return s_renderer->LoadShaderStorageBuffer(data, size);
}

void RenderEngine::LoadShaderStorageBuffer(ShaderStorageBufferHandle handle, uint8_t* data, uint32_t size) {
	s_renderer->LoadShaderStorageBuffer(handle, data, size);
}

void RenderEngine::BindShaderStorageBuffer(ShaderStorageBufferHandle handle, uint32_t index) {
	s_renderer->BindShaderStorageBuffer(handle, index);
}

uint32_t RenderEngine::GetShaderStorageBufferSize(ShaderStorageBufferHandle handle) {
	return s_renderer->GetShaderStorageBufferSize(handle);
}

std::vector<uint8_t> RenderEngine::GetShaderStorageBufferData(ShaderStorageBufferHandle handle, uint32_t offset, uint32_t size) {
	return s_renderer->GetShaderStorageBufferData(handle, offset, size);
}

ShaderHandle RenderEngine::CreateShader(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
	return s_renderer->CreateShader(vertexShaderSource, fragmentShaderSource);
}

void RenderEngine::SetUniform1i(ShaderHandle handle, const std::string& name, int32_t value) {
	s_renderer->SetUniform1i(handle, name, value);
}

void RenderEngine::SetUniform1ui(ShaderHandle handle, const std::string& name, uint32_t value) {
	s_renderer->SetUniform1ui(handle, name, value);
}

void RenderEngine::SetUniform2i(ShaderHandle handle, const std::string& name, glm::ivec2 v) {
	s_renderer->SetUniform2i(handle, name, v);
}

void RenderEngine::SetUniform1iv(ShaderHandle handle, const std::string& name, int32_t* data, int32_t count) {
	s_renderer->SetUniform1iv(handle, name, data, count);
}

void RenderEngine::SetUniform1f(ShaderHandle handle, const std::string& name, float value) {
	s_renderer->SetUniform1f(handle, name, value);
}

void RenderEngine::SetUniform1fv(ShaderHandle handle, const std::string& name, float* data, int32_t count) {
	s_renderer->SetUniform1fv(handle, name, data, count);
}

void RenderEngine::SetUniform2f(ShaderHandle handle, const std::string& name, glm::vec2 v) {
	s_renderer->SetUniform2f(handle, name, v);
}

void RenderEngine::SetUniform2fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform2fv(handle, name, start, count);
}

void RenderEngine::SetUniform3f(ShaderHandle handle, const std::string& name, glm::vec3 v) {
	s_renderer->SetUniform3f(handle, name, v);
}

void RenderEngine::SetUniform3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform3fv(handle, name, start, count);
}

void RenderEngine::SetUniform4f(ShaderHandle handle, const std::string& name, glm::vec4 v) {
	s_renderer->SetUniform4f(handle, name, v);
}

void RenderEngine::SetUniform4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform4fv(handle, name, start, count);
}

void RenderEngine::SetUniformMat3f(ShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose) {
	s_renderer->SetUniformMat3f(handle, name, m, transpose);
}

void RenderEngine::SetUniformMat3fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose) {
	s_renderer->SetUniformMat3fv(handle, name, start, count, transpose);
}

void RenderEngine::SetUniformMat4f(ShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose) {
	s_renderer->SetUniformMat4f(handle, name, m, transpose);
}

void RenderEngine::SetUniformMat4fv(ShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose) {
	s_renderer->SetUniformMat4fv(handle, name, start, count, transpose);
}

void RenderEngine::BindTexture(ShaderHandle shaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index) {
	s_renderer->BindTexture(shaderHandle, name, textureHandle, index);
}

ComputeShaderHandle RenderEngine::CreateComputeShader(const std::string& source) {
	return s_renderer->CreateComputeShader(source);
}

void RenderEngine::DispatchComputeShader(ComputeShaderHandle handle, int32_t x, int32_t y, int32_t z) {
	s_renderer->DispatchComputeShader(handle, x, y, z);
}

void RenderEngine::SetUniform1i(ComputeShaderHandle handle, const std::string& name, int32_t value) {
	s_renderer->SetUniform1i(handle, name, value);
}

void RenderEngine::SetUniform1ui(ComputeShaderHandle handle, const std::string& name, uint32_t value) {
	s_renderer->SetUniform1ui(handle, name, value);
}

void RenderEngine::SetUniform2i(ComputeShaderHandle handle, const std::string& name, glm::ivec2 v) {
	s_renderer->SetUniform2i(handle, name, v);
}

void RenderEngine::SetUniform1iv(ComputeShaderHandle handle, const std::string& name, int32_t* data, int32_t count) {
	s_renderer->SetUniform1iv(handle, name, data, count);
}

void RenderEngine::SetUniform1f(ComputeShaderHandle handle, const std::string& name, float value) {
	s_renderer->SetUniform1f(handle, name, value);
}

void RenderEngine::SetUniform1fv(ComputeShaderHandle handle, const std::string& name, float* data, int32_t count) {
	s_renderer->SetUniform1fv(handle, name, data, count);
}

void RenderEngine::SetUniform2f(ComputeShaderHandle handle, const std::string& name, glm::vec2 v) {
	s_renderer->SetUniform2f(handle, name, v);
}

void RenderEngine::SetUniform2fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform2fv(handle, name, start, count);
}

void RenderEngine::SetUniform3f(ComputeShaderHandle handle, const std::string& name, glm::vec3 v) {
	s_renderer->SetUniform3f(handle, name, v);
}

void RenderEngine::SetUniform3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform3fv(handle, name, start, count);
}

void RenderEngine::SetUniform4f(ComputeShaderHandle handle, const std::string& name, glm::vec4 v) {
	s_renderer->SetUniform4f(handle, name, v);
}

void RenderEngine::SetUniform4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count) {
	s_renderer->SetUniform4fv(handle, name, start, count);
}

void RenderEngine::SetUniformMat3f(ComputeShaderHandle handle, const std::string& name, const glm::mat3& m, bool transpose) {
	s_renderer->SetUniformMat3f(handle, name, m, transpose);
}

void RenderEngine::SetUniformMat3fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose) {
	s_renderer->SetUniformMat3fv(handle, name, start, count, transpose);
}

void RenderEngine::SetUniformMat4f(ComputeShaderHandle handle, const std::string& name, const glm::mat4& m, bool transpose) {
	s_renderer->SetUniformMat4f(handle, name, m, transpose);
}

void RenderEngine::SetUniformMat4fv(ComputeShaderHandle handle, const std::string& name, const float* start, int32_t count, bool transpose) {
	s_renderer->SetUniformMat4fv(handle, name, start, count, transpose);
}

void RenderEngine::BindTexture(ComputeShaderHandle computeShaderHandle, const std::string& name, TextureHandle textureHandle, uint64_t index) {
	s_renderer->BindTexture(computeShaderHandle, name, textureHandle, index);
}

void RenderEngine::MemoryBarriersAll() {
	s_renderer->MemoryBarriersAll();
}

uint64_t RenderEngine::GetInternalID(TextureHandle handle) {
	return s_renderer->GetInternalID(handle);
}

uint64_t RenderEngine::GetInternalColorAttachmentID(FrameBufferHandle handle) {
	return s_renderer->GetInternalColorAttachmentID(handle);
}

uint64_t RenderEngine::GetInternalDepthAttachmentID(FrameBufferHandle handle) {
	return s_renderer->GetInternalDepthAttachmentID(handle);
}
