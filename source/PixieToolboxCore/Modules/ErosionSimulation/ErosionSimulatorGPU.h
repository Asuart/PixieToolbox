#pragma once
#include <cstdint>
#include <cmath>
#include <vector>
#include <array>
#include <random>
#include <format>

#include "Rendering/RenderEngine.h"
#include "Utils/StringUtils.h"

static const char* INCREMENT_WATER_SHADER_SOURCE = R"(
void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	water_height[index] += PREPROC_WATER_INCREMENT * PREPROC_DT;
}
)";

static const char* UPDATE_OUTFLOW_FLUX_SHADER_SOURCE = R"(
void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}
	
	float outflowLeft = outflow_flux_left[index];
	float outflowRight = outflow_flux_right[index];
	float outflowTop = outflow_flux_top[index];
	float outflowBottom = outflow_flux_bottom[index];

	float terrainHeight = terrain_height[index];
	float waterHeight = water_height[index];
	float cellHeight = terrainHeight + waterHeight;

	if ((index % PREPROC_RESOLUTION_X) == 0) {
		outflowLeft = 0;
	}
	else {
		float waterHeightLeft =  water_height[index - 1];
		float cellheightLeft = terrain_height[index - 1] + waterHeightLeft;
		float deltaHeight = cellHeight - cellheightLeft;
		float averageWaterHeight = (waterHeight + waterHeightLeft) * 0.5f; 
		float crossSection = averageWaterHeight * PREPROC_CELL_SIZE_Y;
		outflowLeft = max(0.0f, outflowLeft + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
	}

	if (((index + 1) % PREPROC_RESOLUTION_X) == 0) {
		outflowRight = 0;
	}
	else {
		float waterHeightRight = water_height[index + 1];
		float cellheightRight = terrain_height[index + 1] + waterHeightRight;
		float deltaHeight = cellHeight - cellheightRight;
		float averageWaterHeight = (waterHeight + waterHeightRight) * 0.5f;
		float crossSection = averageWaterHeight * PREPROC_CELL_SIZE_Y;
		outflowRight = max(0.0f, outflowRight + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
	}

	if ((index / PREPROC_RESOLUTION_X) == 0) {
		outflowTop = 0;
	}
	else {
		float waterHeightTop = water_height[index - PREPROC_RESOLUTION_X];
		float cellheightTop = terrain_height[index - PREPROC_RESOLUTION_X] + waterHeightTop;
		float deltaHeight = cellHeight - cellheightTop;
		float averageWaterHeight = (waterHeight + waterHeightTop) * 0.5f;
		float crossSection = averageWaterHeight * PREPROC_CELL_SIZE_X;
		outflowTop = max(0.0f, outflowTop + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
	}

	if (((index / PREPROC_RESOLUTION_X) + 1) == PREPROC_RESOLUTION_Y) {
		outflowBottom = 0;
	}
	else {
		float waterHeightBottom = water_height[index + PREPROC_RESOLUTION_X];
		float cellheightBottom = terrain_height[index + PREPROC_RESOLUTION_X] + waterHeightBottom;
		float deltaHeight = cellHeight - cellheightBottom;
		float averageWaterHeight = (waterHeight + waterHeightBottom) * 0.5f;
		float crossSection = averageWaterHeight * PREPROC_CELL_SIZE_X;
		outflowBottom = max(0.0f, outflowBottom + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
	}

	float totalOutflow = outflowLeft + outflowRight + outflowTop + outflowBottom;
	float maxOutPerFace = waterHeight * PREPROC_CELL_AREA / PREPROC_DT;

	outflowLeft  = min(outflowLeft,  maxOutPerFace);
	outflowRight = min(outflowRight, maxOutPerFace);
	outflowTop   = min(outflowTop,   maxOutPerFace);
	outflowBottom= min(outflowBottom,maxOutPerFace);

	if (totalOutflow > maxOutPerFace) {
		float scale = maxOutPerFace / totalOutflow;
		outflowLeft  *= scale;
		outflowRight *= scale;
		outflowTop   *= scale;
		outflowBottom*= scale;
	}

	float outflowLimitScale = min(1.0f, waterHeight * PREPROC_CELL_AREA / (totalOutflow * PREPROC_DT));

	outflow_flux_left[index] = outflowLeft * outflowLimitScale;
	outflow_flux_right[index] = outflowRight * outflowLimitScale;
	outflow_flux_top[index] = outflowTop * outflowLimitScale;
	outflow_flux_bottom[index] = outflowBottom * outflowLimitScale;
}
)";

static const char* UPDATE_WATER_SURFACE_AND_VELOCITY_SHADER_SOURCE = R"(
void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	float initialWaterHeight = water_height[index];

	float outflowLeft = outflow_flux_left[index];
	float inflowLeft = 0.0f;
	if ((index % PREPROC_RESOLUTION_X) != 0) {
		inflowLeft = outflow_flux_right[index - 1];
	}

	float outflowRight = outflow_flux_right[index];
	float inflowRight = 0.0f;
	if (((index + 1) % PREPROC_RESOLUTION_X) != 0) {
		inflowRight = outflow_flux_left[index + 1];
	}

	float outflowTop = outflow_flux_top[index];
	float inflowTop = 0.0f;
	if ((index / PREPROC_RESOLUTION_X) != 0) {
		inflowTop = outflow_flux_bottom[index - PREPROC_RESOLUTION_X];
	}

	float outflowBottom = outflow_flux_bottom[index];
	float inflowBottom = 0.0f;
	if (((index / PREPROC_RESOLUTION_X) + 1) != PREPROC_RESOLUTION_Y) {
		inflowBottom = outflow_flux_top[index + PREPROC_RESOLUTION_X];
	}

	float totalOutflow = outflowLeft + outflowRight + outflowTop + outflowBottom;
	float totalInflow = inflowLeft + inflowRight + inflowTop + inflowBottom;
	float volumeChange = (totalInflow - totalOutflow) * PREPROC_DT;

	float updatedWaterHeight = max(0.0f, initialWaterHeight + volumeChange * PREPROC_INVERSE_CELL_AREA);
	water_height[index] = updatedWaterHeight;

	float averageWaterHeight = (initialWaterHeight + updatedWaterHeight) * 0.5f;
	if (averageWaterHeight < 0.001f) {
		velocity_x[index] = 0.0f;
		velocity_y[index] = 0.0f;
		return;
	}

	float waterFlowX = (inflowLeft - outflowLeft + outflowRight - inflowRight) * 0.5f;
	float waterFlowY = (inflowTop - outflowTop + outflowBottom - inflowBottom) * 0.5f;

	float velocityX = min(waterFlowX / (PREPROC_CELL_SIZE_Y * averageWaterHeight), 0.99f / PREPROC_DT);
	float velocityY = min(waterFlowY / (PREPROC_CELL_SIZE_X * averageWaterHeight), 0.99f / PREPROC_DT);

	velocity_x[index] = velocityX;
	velocity_y[index] = velocityY;
}
)";

static const char* UPDATE_EROSION_AND_DEPOSITION_SHADER_SOURCE = R"(
uint CoordsToIndexClamped(uint x, uint y) {
	if (x < 0) x = 0;
	else if (x >= PREPROC_RESOLUTION_X) x = PREPROC_RESOLUTION_X - 1;
	if (y < 0) y = 0;
	else if (y >= PREPROC_RESOLUTION_Y) y = PREPROC_RESOLUTION_Y - 1;
	return y * PREPROC_RESOLUTION_X + x;
}

float SampleTiltSin(uint x, uint y) {
	float heightLeft = terrain_height[CoordsToIndexClamped(x - 1, y)];
	float heightRight = terrain_height[CoordsToIndexClamped(x + 1, y)];
	float heightTop = terrain_height[CoordsToIndexClamped(x, y - 1)];
	float heightBottom = terrain_height[CoordsToIndexClamped(x, y + 1)];
	float dx = (heightRight - heightLeft) / (2.0 * PREPROC_CELL_SIZE_X);
	float dy = (heightBottom - heightTop) / (2.0 * PREPROC_CELL_SIZE_Y);

	vec3 dhdx = vec3(2 * PREPROC_CELL_SIZE_X, heightRight - heightLeft, 0);
	vec3 dhdy = vec3(0, heightTop - heightBottom, 2 * PREPROC_CELL_SIZE_Y);
	vec3 normal = cross(dhdx, dhdy);
	float sinTiltAngle = abs(normal.y) / length(normal);

	return sinTiltAngle;
}

float SampleSpeed(uint cellIndex) {
	return sqrt(velocity_x[cellIndex] * velocity_x[cellIndex] + velocity_y[cellIndex] * velocity_y[cellIndex]);
}

void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	float tiltSin = SampleTiltSin(x, y);
	float tiltScale = clamp(tiltSin, 0.1f, 0.5f);
	float speed = SampleSpeed(index);
	float capacity = PREPROC_SEDIMENT_CAPACITY_CONSTANT * tiltScale * speed;
	float suspendedSediment = suspended_sediment[index];

	float Dmax = 5.0;
	float lmax = clamp(1.0 - max(0.0, Dmax - water_height[index]) / Dmax, 0.0, 1.0);
	capacity *= lmax;

	if (capacity > suspendedSediment) {
		float dissolvedAmount = min(
			PREPROC_DT * PREPROC_DISSOLVE_SEDIMENT_CONSTANT * (capacity - suspendedSediment),
			terrain_height[index] * 0.01
		);
		terrain_height_updated[index] = max(0.0f, terrain_height[index] - dissolvedAmount);
		suspended_sediment[index] += dissolvedAmount;
	}
	else {
		float depositedAmount = min(
			PREPROC_DT * PREPROC_SEDIMENT_DEPOSITION_CONSTANT * (suspendedSediment - capacity),
			terrain_height[index] * 0.01
		);
		terrain_height_updated[index] = terrain_height[index] + depositedAmount;
		suspended_sediment[index] -= depositedAmount;
	}
}
)";

static const char* SEDIMENT_TRANSPORT_SHADER_SOURCE = R"(
uint CoordsToIndexClamped(uint x, uint y) {
	if (x < 0) x = 0;
	else if (x >= PREPROC_RESOLUTION_X) x = PREPROC_RESOLUTION_X - 1;
	if (y < 0) y = 0;
	else if (y >= PREPROC_RESOLUTION_Y) y = PREPROC_RESOLUTION_Y - 1;
	return y * PREPROC_RESOLUTION_X + x;
}

void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	terrain_height[index] = terrain_height_updated[index];

	float offsetX = -velocity_x[index] * PREPROC_DT;
	float offsetY = -velocity_y[index] * PREPROC_DT;
	float absOffsetX = min(1.0f, abs(offsetX));
	float absOffsetY = min(1.0f, abs(offsetY));
	int intOffsetX = offsetX >= 0 ? 1 : -1;
	int intOffsetY = offsetY >= 0 ? 1 : -1;

	float v00 = suspended_sediment[index];
	float v10 = suspended_sediment[CoordsToIndexClamped(x + intOffsetX, y)];
	float v01 = suspended_sediment[CoordsToIndexClamped(x, y + intOffsetY)];
	float v11 = suspended_sediment[CoordsToIndexClamped(x + intOffsetX, y + intOffsetY)];

	float bilinearlyInterpolated = 
		v00 * (1.0f - absOffsetX) * (1.0f - absOffsetY) +
		v10 * absOffsetX * (1.0f - absOffsetY) +
		v01 * (1.0f - absOffsetX) * absOffsetY +
		v11 * absOffsetX * absOffsetY;

	suspended_sediment_updated[index] = bilinearlyInterpolated;
}
)";

static const char* EVAPORATION_SHADER_SOURCE = R"(
void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	water_height[index] = max(0.0f, water_height[index] - water_height[index] * PREPROC_DT * PREPROC_EVAPORATION_CONSTANT);
	suspended_sediment[index] = suspended_sediment_updated[index];
}
)";

class ErosionSimulatorGPU {
public:
	ErosionSimulatorGPU(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
		Resize(resolutionX, resolutionY, realSizeX, realSizeY);
	}

	void Run(int32_t steps) {
		for (int32_t i = 0; i < steps; i++) {
			Step();
		}
	}

	void Step() {
		uint32_t workGroupsX = m_resolutionX / m_localWorkGroupSizeX;
		if (m_resolutionX % m_localWorkGroupSizeX) {
			workGroupsX++;
		}
		uint32_t workGroupsY = m_resolutionY / m_localWorkGroupSizeY;
		if (m_resolutionY % m_localWorkGroupSizeY) {
			workGroupsY++;
		}

		RenderEngine::DispatchComputeShader(m_incrementWaterShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		RenderEngine::DispatchComputeShader(m_updateOutflowFluxShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		RenderEngine::DispatchComputeShader(m_updateWaterSurfaceAndVelocityShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		RenderEngine::DispatchComputeShader(m_updateErosionAndDepositionShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		RenderEngine::DispatchComputeShader(m_sedimentTransportShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		RenderEngine::DispatchComputeShader(m_evaporationShader, workGroupsX, workGroupsY, 1);
		RenderEngine::MemoryBarriersAll();
		m_iteration++;
	}

	void Resize(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
		m_resolutionX = resolutionX;
		m_resolutionY = resolutionY;
		m_realSizeX = realSizeX;
		m_realSizeY = realSizeY;
		m_cellsCount = m_resolutionX * m_resolutionY;
		m_cellSizeX = m_realSizeX / static_cast<float>(m_resolutionX);
		m_cellSizeY = m_realSizeY / static_cast<float>(m_resolutionY);
		m_cellArea = m_cellSizeX * m_cellSizeY;
		m_iteration = 0;

		m_pipeLength = m_cellSizeX;
		m_inverseCellArea = 1.0f / m_cellArea;
		m_dtGravity_pipeLength = m_dt * m_gravity / m_pipeLength;
		m_dtEvaporationConstant = m_dt * m_evaporationConstant;
	}

	void SetInitialHeightMap(float* heightMapData, int32_t resolutionX, int32_t resolutionY) {
		Resize(resolutionX, resolutionY, m_realSizeX, m_realSizeY);
		std::vector<uint8_t> zeroes(m_cellsCount * sizeof(float));
		std::fill(zeroes.begin(), zeroes.end(), 0);

		m_terrainHeight = RenderEngine::LoadShaderStorageBuffer((uint8_t*)heightMapData, resolutionX * resolutionY * sizeof(float));
		m_terrainHeightUpdated = RenderEngine::LoadShaderStorageBuffer((uint8_t*)heightMapData, resolutionX * resolutionY * sizeof(float));
		m_waterHeight = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_suspendedSediment = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_suspendedSedimentUpdated = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_outflowFluxLeft = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_outflowFluxRight = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_outflowFluxTop = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_outflowFluxBottom = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_velocityX = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));
		m_velocityY = RenderEngine::LoadShaderStorageBuffer(zeroes.data(), resolutionX * resolutionY * sizeof(float));

		m_incrementWaterShader = RenderEngine::CreateComputeShader(PreprocessShader(INCREMENT_WATER_SHADER_SOURCE));
		m_updateOutflowFluxShader = RenderEngine::CreateComputeShader(PreprocessShader(UPDATE_OUTFLOW_FLUX_SHADER_SOURCE));
		m_updateWaterSurfaceAndVelocityShader = RenderEngine::CreateComputeShader(PreprocessShader(UPDATE_WATER_SURFACE_AND_VELOCITY_SHADER_SOURCE));
		m_updateErosionAndDepositionShader = RenderEngine::CreateComputeShader(PreprocessShader(UPDATE_EROSION_AND_DEPOSITION_SHADER_SOURCE));
		m_sedimentTransportShader = RenderEngine::CreateComputeShader(PreprocessShader(SEDIMENT_TRANSPORT_SHADER_SOURCE));
		m_evaporationShader = RenderEngine::CreateComputeShader(PreprocessShader(EVAPORATION_SHADER_SOURCE));

		RenderEngine::BindShaderStorageBuffer(m_terrainHeight, 0);
		RenderEngine::BindShaderStorageBuffer(m_terrainHeightUpdated, 1);
		RenderEngine::BindShaderStorageBuffer(m_waterHeight, 2);
		RenderEngine::BindShaderStorageBuffer(m_suspendedSediment, 3);
		RenderEngine::BindShaderStorageBuffer(m_suspendedSedimentUpdated, 4);
		RenderEngine::BindShaderStorageBuffer(m_outflowFluxLeft, 5);
		RenderEngine::BindShaderStorageBuffer(m_outflowFluxRight, 6);
		RenderEngine::BindShaderStorageBuffer(m_outflowFluxTop, 7);
		RenderEngine::BindShaderStorageBuffer(m_outflowFluxBottom, 8);
		RenderEngine::BindShaderStorageBuffer(m_velocityX, 9);
		RenderEngine::BindShaderStorageBuffer(m_velocityY, 10);
	}

public:
	ComputeShaderHandle m_incrementWaterShader;
	ComputeShaderHandle m_updateOutflowFluxShader;
	ComputeShaderHandle m_updateWaterSurfaceAndVelocityShader;
	ComputeShaderHandle m_updateErosionAndDepositionShader;
	ComputeShaderHandle m_sedimentTransportShader;
	ComputeShaderHandle m_evaporationShader;

	ShaderStorageBufferHandle m_terrainHeight;
	ShaderStorageBufferHandle m_terrainHeightUpdated;
	ShaderStorageBufferHandle m_waterHeight;
	ShaderStorageBufferHandle m_suspendedSediment;
	ShaderStorageBufferHandle m_suspendedSedimentUpdated;
	ShaderStorageBufferHandle m_outflowFluxLeft;
	ShaderStorageBufferHandle m_outflowFluxRight;
	ShaderStorageBufferHandle m_outflowFluxTop;
	ShaderStorageBufferHandle m_outflowFluxBottom;
	ShaderStorageBufferHandle m_velocityX;
	ShaderStorageBufferHandle m_velocityY;

	int32_t m_resolutionX;
	int32_t m_resolutionY;
	float m_realSizeX;
	float m_realSizeY;
	int64_t m_iteration;

	// constants
	float m_dt = 0.005f;
	float m_gravity = 9.8f;
	float m_sedimentCapacityConstant = 1.0f;
	float m_sedimentDissolveConstant = 0.05f;
	float m_sedimentDepositionConstant = 0.05f;
	float m_minTiltScale = 0.1f;
	float m_evaporationConstant = 1.0f;
	float m_waterIncrement = 0.2f;

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

	enum BufferBinding : uint32_t {
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

	std::string PreprocessShader(const std::string& src) {
		const std::string shaderVersion = "#version 430 core\n\n";
		const std::string shaderWorkgroupSize = std::format("layout(local_size_x = {}, local_size_y = {}, local_size_z = 1) in;\n\n", m_localWorkGroupSizeX, m_localWorkGroupSizeY);

		const std::vector<std::array< std::string, 2>> bindingsData = {
			{ "terrain_height",				"float" },
			{ "terrain_height_updated",		"float" },
			{ "water_height",				"float" },
			{ "suspended_sediment",			"float" },
			{ "suspended_sediment_updated", "float" },
			{ "outflow_flux_left",			"float" },
			{ "outflow_flux_right",			"float" },
			{ "outflow_flux_top",			"float" },
			{ "outflow_flux_bottom",		"float" },
			{ "velocity_x",					"float" },
			{ "velocity_y",					"float" }
		};

		std::string assembledSource = shaderVersion + shaderWorkgroupSize;
		for (int32_t bufferIndex = 0; bufferIndex < (int32_t)BufferBinding::Count; bufferIndex++) {
			if (src.find(bindingsData[bufferIndex][0]) == -1) continue;
			std::string bufferCode = std::to_string(bufferIndex);
			std::string bindingCode =
				std::format("layout(std430, binding = {}) buffer {}_buffer ", bufferCode, bindingsData[bufferIndex][0]) +
				"{\n" +
				std::format("{} {}[];", bindingsData[bufferIndex][1], bindingsData[bufferIndex][0]) +
				"\n};\n\n";
			assembledSource += bindingCode;
		}

		std::string preprocessed = src;
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_RESOLUTION_X", std::to_string(m_resolutionX));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_RESOLUTION_Y", std::to_string(m_resolutionY));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_DT", std::to_string(m_dt));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_WATER_INCREMENT", std::to_string(m_waterIncrement));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_CELL_AREA", std::to_string(m_cellArea));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_CELL_SIZE_X", std::to_string(m_cellSizeX));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_CELL_SIZE_Y", std::to_string(m_cellSizeY));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_INVERSE_CELL_AREA", std::to_string(m_inverseCellArea));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_GRAVITY", std::to_string(m_gravity));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_PIPE_LENGTH", std::to_string(m_pipeLength));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_SEDIMENT_CAPACITY_CONSTANT", std::to_string(m_sedimentCapacityConstant));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_DISSOLVE_SEDIMENT_CONSTANT", std::to_string(m_sedimentDissolveConstant));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_SEDIMENT_DEPOSITION_CONSTANT", std::to_string(m_sedimentDepositionConstant));
		preprocessed = StringUtils::ReplaceAll(preprocessed, "PREPROC_EVAPORATION_CONSTANT", std::to_string(m_evaporationConstant));

		assembledSource += preprocessed;

		//std::cout << assembledSource << " \n\n\n";

		return assembledSource;
	}
};
