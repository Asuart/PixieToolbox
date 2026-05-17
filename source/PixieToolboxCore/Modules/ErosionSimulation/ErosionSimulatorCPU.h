#pragma once
#include <cstdint>
#include <vector>

class ErosionSimulatorCPU {
public:
	ErosionSimulatorCPU(int32_t resolutionX = 1, int32_t resolutionY = 1, float realSizeX = 1.0f, float realSizeY = 1.0f);

	void Run(int32_t steps);
	void Step();

	void Resize(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY);

	void SetInitialHeightMap(const float* const heightMapData, int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY);
    void InitializeUniformHeightMap(float height);
    void RandomizeInitialHeighMap(float minHeight, float maxHeight);

public:
	std::vector<float> m_terrainHeight;
	std::vector<float> m_waterHeight;
	std::vector<float> m_suspendedSediment;
	std::vector<float> m_suspendedSedimentUpdated;
	std::vector<float> m_outflowFluxLeft;
	std::vector<float> m_outflowFluxRight;
	std::vector<float> m_outflowFluxTop;
	std::vector<float> m_outflowFluxBottom;
	std::vector<float> m_velocityX;
	std::vector<float> m_velocityY;
	int32_t m_resolutionX;
	int32_t m_resolutionY;
	float m_realSizeX;
	float m_realSizeY;
	int64_t m_iteration;

	// constants
	float m_dt = 0.001f;
	float m_gravity = 9.8f;
	float m_sedimentCapacityConstant = 0.05f;
	float m_sedimentDissolveConstant = 0.1f;
	float m_sedimentDepositionConstant = 0.1f;
	float m_minTiltScale = 0.01f;
    float m_maxTiltScale = 1.0f;
	float m_evaporationConstant = 0.2f;
    float m_waterIncrement = 1.2f;

	// precomputed values
	int64_t m_cellsCount;
	float m_pipeLength;
	float m_cellSizeX;
	float m_cellSizeY;
	float m_cellArea;
	float m_inverseCellArea;
	float m_dtGravity_pipeLength;
	float m_dtEvaporationConstant;

	void IncrementWater();
	void UpdateOutflowFlux();
	void UpdateWaterSurfaceAndVelocity();
	void UpdateErosionAndDeposition();
	void SedimentTransport();
	void Evaporation();

	float SampleTilt(int64_t x, int64_t y);
	float SampleSpeed(int64_t cellIndex);

	int64_t CoordsToIndexClamped(int64_t x, int64_t y);

	bool IsReasonable(float value);
};
