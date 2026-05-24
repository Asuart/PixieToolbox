#pragma once
#include <cstdint>
#include <array>
#include <string>

#include "Rendering/ResourceHandles.h"
#include "Utils/ShaderPreprocessor.h"

class ErosionSimulatorGPU {
public:
	enum class BufferBinding : uint32_t {
		TerrainHeight = 0,
		TerrainHeightUpdated,
		WaterHeight,
		SuspendedSediment,
		SuspendedSedimentUpdated,
		OutflowFluxLeft,
		OutflowFluxRight,
		OutflowFluxTop,
		OutflowFluxBottom,
		VelocityX,
		VelocityY,
		Count
	};

	ErosionSimulatorGPU(int32_t resolutionX = 1, int32_t resolutionY = 1, float realSizeX = 1.0f, float realSizeY = 1.0f);

	void Run(int32_t steps);
	void Step();

	void Resize(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY);

	void SetInitialHeightMap(float* heightMapData, int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY);

public:
	std::array<ShaderStorageBufferHandle, static_cast<size_t>(BufferBinding::Count)> m_buffers;
	ShaderPreprocessor m_shaderPreprocessor;

	ComputeShaderHandle m_incrementWaterShader;
	ComputeShaderHandle m_updateOutflowFluxShader;
	ComputeShaderHandle m_updateWaterSurfaceAndVelocityShader;
	ComputeShaderHandle m_updateErosionAndDepositionShader;
	ComputeShaderHandle m_sedimentTransportShader;
	ComputeShaderHandle m_evaporationShader;

	int32_t m_resolutionX;
	int32_t m_resolutionY;
	float m_realSizeX;
	float m_realSizeY;
	int64_t m_iteration;

	// constants
	float m_dt = 0.01f;
	float m_gravity = 9.8f;
	float m_sedimentCapacityConstant = 0.15f;
	float m_sedimentDissolveConstant = 0.01f;
	float m_sedimentDepositionConstant = 0.01f;
	float m_evaporationConstant = 1.2f;
	float m_waterIncrement = 0.15f;
	float m_fluxDamping = 0.999f;

	// precomputed values
	int64_t m_cellsCount;
	float m_pipeLength;
	float m_cellSizeX;
	float m_cellSizeY;
	float m_cellArea;
	float m_inverseCellArea;
	float m_dtGravity_pipeLength;
	float m_dtEvaporationConstant;

	int32_t m_localWorkGroupSizeX = 16;
	int32_t m_localWorkGroupSizeY = 16;
	int32_t m_workGroupsX;
	int32_t m_workGroupsY;
};
