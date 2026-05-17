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
	MeshHandle m_gridMesh;
	ShaderHandle m_terrainShader;
	ShaderHandle m_waterShader;
	Camera m_camera;
	FreeCameraController m_cameraController;



	ErosionSimulatorGPU m_sim;
};
