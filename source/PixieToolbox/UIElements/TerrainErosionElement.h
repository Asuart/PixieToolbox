#pragma once
#include "Window/UIElement.h"
#include "Scene/FreeCameraController.h"
#include "Rendering/ResourceHandles.h"
#include "Modules/ErosionSimulation/ErosionSimulatorGPU.h"

class TerrainErosionElement : public UIElement {
public:
	TerrainErosionElement();

	void Draw();
	void HandleEvent(const SDL_Event& event) override;

protected:
	bool m_focused = false;
	FrameBufferHandle m_frameBuffer;
	ShaderHandle m_terrainShader;
	MeshHandle m_terrainMesh;
	TextureHandle m_terrainHeight;
	Camera m_camera;
	FreeCameraController m_cameraController;



	ErosionSimulatorGPU m_sim;
};
