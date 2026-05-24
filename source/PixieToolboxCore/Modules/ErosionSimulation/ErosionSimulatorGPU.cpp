#include "ErosionSimulatorGPU.h"

#include <vector>
#include <random>
#include <format>
#include <cmath>

#include "Utils/StringUtils.h"
#include "Rendering/RenderEngine.h"

static const char* INCREMENT_WATER_SHADER_SOURCE = R"(
float hash(uint seed) {
    seed = (seed ^ 61u) ^ (seed >> 16u);
    seed *= 9u;
    seed = seed ^ (seed >> 4u);
    seed *= 0x27d4eb2du;
    seed = seed ^ (seed >> 15u);
    return float(seed) * (1.0 / 4294967296.0);
}

void main() {
	const uint x = gl_GlobalInvocationID.x;
	const uint y = gl_GlobalInvocationID.y;
	const uint index = y * PREPROC_RESOLUTION_X + x;
	if (x >= PREPROC_RESOLUTION_X || y >= PREPROC_RESOLUTION_Y) {
		return;
	}

	water_height[index] += (1.0f - 0.15f + 0.3f * hash(index + uint(water_height[index]))) * PREPROC_WATER_INCREMENT * PREPROC_DT;
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
		outflowLeft = max(0.0f, outflowLeft * PREPROC_FLUX_DAMPING + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
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
		outflowRight = max(0.0f, outflowRight * PREPROC_FLUX_DAMPING + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
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
		outflowTop = max(0.0f, outflowTop * PREPROC_FLUX_DAMPING + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
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
		outflowBottom = max(0.0f, outflowBottom * PREPROC_FLUX_DAMPING + deltaHeight * crossSection * PREPROC_DT * PREPROC_GRAVITY / PREPROC_PIPE_LENGTH);
	}

	float maxOutPerFace = waterHeight * PREPROC_CELL_AREA / PREPROC_DT;

	outflowLeft = min(outflowLeft, maxOutPerFace);
	outflowRight = min(outflowRight, maxOutPerFace);
	outflowTop = min(outflowTop, maxOutPerFace);
	outflowBottom = min(outflowBottom,maxOutPerFace);

	float totalOutflow = outflowLeft + outflowRight + outflowTop + outflowBottom;

	if (totalOutflow > maxOutPerFace) {
		float scale = maxOutPerFace / totalOutflow;
		outflowLeft *= scale;
		outflowRight *= scale;
		outflowTop *= scale;
		outflowBottom *= scale;
	}

	outflow_flux_left[index] = outflowLeft;
	outflow_flux_right[index] = outflowRight;
	outflow_flux_top[index] = outflowTop;
	outflow_flux_bottom[index] = outflowBottom;
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
	float averageWaterHeight = (initialWaterHeight + updatedWaterHeight) * 0.5f;
	water_height[index] = updatedWaterHeight;

	float waterFlowX = (inflowLeft - outflowLeft + outflowRight - inflowRight) * 0.5f;
	float waterFlowY = (inflowTop - outflowTop + outflowBottom - inflowBottom) * 0.5f;

	float velocityX = waterFlowX / (PREPROC_CELL_SIZE_Y * averageWaterHeight);
	float velocityY = waterFlowY / (PREPROC_CELL_SIZE_X * averageWaterHeight);

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

	vec3 dhdx = vec3(2 * PREPROC_CELL_SIZE_X, heightRight - heightLeft, 0);
	vec3 dhdy = vec3(0, heightTop - heightBottom, 2 * PREPROC_CELL_SIZE_Y);
	vec3 normal = cross(dhdx, dhdy);

	float tiltCos = abs(normal.y) / length(normal);

	return sqrt(1.0f - tiltCos * tiltCos);
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
	float speed = SampleSpeed(index);
	float capacity = PREPROC_SEDIMENT_CAPACITY_CONSTANT * tiltSin * speed * water_height[index];
	float suspendedSediment = suspended_sediment[index];

	if (capacity > suspendedSediment) {
		float dissolvedAmount = PREPROC_DT * PREPROC_DISSOLVE_SEDIMENT_CONSTANT * (capacity - suspendedSediment);
		terrain_height_updated[index] = terrain_height[index] - dissolvedAmount;
		suspended_sediment[index] += dissolvedAmount;
	}
	else {
		float depositedAmount = PREPROC_DT * PREPROC_SEDIMENT_DEPOSITION_CONSTANT * (suspendedSediment - capacity);
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

	float offsetX = -velocity_x[index] * PREPROC_DT / PREPROC_CELL_SIZE_X;
	float offsetY = -velocity_y[index] * PREPROC_DT / PREPROC_CELL_SIZE_Y;
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

	suspended_sediment[index] = suspended_sediment_updated[index];
	water_height[index] = max(0.0f, water_height[index] - water_height[index] * PREPROC_DT * PREPROC_EVAPORATION_CONSTANT);
}
)";

ErosionSimulatorGPU::ErosionSimulatorGPU(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
	for (int32_t i = 0; i < m_buffers.size(); i++) {
		m_buffers[i] = RenderEngine::LoadShaderStorageBuffer(nullptr, 0);
		RenderEngine::BindShaderStorageBuffer(m_buffers[i], i);
	}

	m_shaderPreprocessor.SetBinding("terrain_height",			  0,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("terrain_height_updated",     1,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("water_height",				  2,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("suspended_sediment",		  3,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("suspended_sediment_updated", 4,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("outflow_flux_left",		  5,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("outflow_flux_right",		  6,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("outflow_flux_top",			  7,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("outflow_flux_bottom",		  8,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("velocity_x",				  9,  ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetBinding("velocity_y",				  10, ShaderPreprocessorType::Float);

    Resize(resolutionX, resolutionY, realSizeX, realSizeY);
}

void ErosionSimulatorGPU::Run(int32_t steps) {
    for (int32_t i = 0; i < steps; i++) {
        Step();
    }
}

void ErosionSimulatorGPU::Step() {
    RenderEngine::DispatchComputeShader(m_incrementWaterShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    RenderEngine::DispatchComputeShader(m_updateOutflowFluxShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    RenderEngine::DispatchComputeShader(m_updateWaterSurfaceAndVelocityShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    RenderEngine::DispatchComputeShader(m_updateErosionAndDepositionShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    RenderEngine::DispatchComputeShader(m_sedimentTransportShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    RenderEngine::DispatchComputeShader(m_evaporationShader, m_workGroupsX, m_workGroupsY, 1);
    RenderEngine::MemoryBarriersAll();
    m_iteration++;
}

void ErosionSimulatorGPU::Resize(int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
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

    m_workGroupsX = m_resolutionX / m_localWorkGroupSizeX;
    if (m_resolutionX % m_localWorkGroupSizeX) {
        m_workGroupsX++;
    }
    m_workGroupsY = m_resolutionY / m_localWorkGroupSizeY;
    if (m_resolutionY % m_localWorkGroupSizeY) {
        m_workGroupsY++;
    }

	std::vector<float> zeroes(m_cellsCount);
	std::fill(zeroes.begin(), zeroes.end(), 0.0f);
	for (int32_t i = 0; i < m_buffers.size(); i++) {
		RenderEngine::LoadShaderStorageBuffer(m_buffers[i], reinterpret_cast<uint8_t*>(zeroes.data()), resolutionX * resolutionY * sizeof(float));
	}
	zeroes.resize(0);

	m_shaderPreprocessor.SetConstant("PREPROC_RESOLUTION_X",				 m_resolutionX,				   ShaderPreprocessorType::Int32);
	m_shaderPreprocessor.SetConstant("PREPROC_RESOLUTION_Y",				 m_resolutionY,				   ShaderPreprocessorType::Int32);
	m_shaderPreprocessor.SetConstant("PREPROC_DT",							 m_dt,						   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_WATER_INCREMENT",				 m_waterIncrement,			   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_CELL_AREA",					 m_cellArea,				   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_CELL_SIZE_X",					 m_cellSizeX,				   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_CELL_SIZE_Y",					 m_cellSizeY,				   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_INVERSE_CELL_AREA",			 m_inverseCellArea,			   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_GRAVITY",						 m_gravity,					   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_PIPE_LENGTH",					 m_pipeLength,				   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_SEDIMENT_CAPACITY_CONSTANT",   m_sedimentCapacityConstant,   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_DISSOLVE_SEDIMENT_CONSTANT",   m_sedimentDissolveConstant,   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_SEDIMENT_DEPOSITION_CONSTANT", m_sedimentDepositionConstant, ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_EVAPORATION_CONSTANT",		 m_evaporationConstant,		   ShaderPreprocessorType::Float);
	m_shaderPreprocessor.SetConstant("PREPROC_FLUX_DAMPING",				 m_fluxDamping,				   ShaderPreprocessorType::Float);

	m_incrementWaterShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(INCREMENT_WATER_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
	m_updateOutflowFluxShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(UPDATE_OUTFLOW_FLUX_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
	m_updateWaterSurfaceAndVelocityShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(UPDATE_WATER_SURFACE_AND_VELOCITY_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
	m_updateErosionAndDepositionShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(UPDATE_EROSION_AND_DEPOSITION_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
	m_sedimentTransportShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(SEDIMENT_TRANSPORT_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
	m_evaporationShader = RenderEngine::CreateComputeShader(
		m_shaderPreprocessor.PreprocessComputeShader(EVAPORATION_SHADER_SOURCE, m_localWorkGroupSizeX, m_localWorkGroupSizeY)
	);
}

void ErosionSimulatorGPU::SetInitialHeightMap(float* heightMapData, int32_t resolutionX, int32_t resolutionY, float realSizeX, float realSizeY) {
    Resize(resolutionX, resolutionY, realSizeX, realSizeY);
	RenderEngine::LoadShaderStorageBuffer(m_buffers[static_cast<uint32_t>(BufferBinding::TerrainHeight)], reinterpret_cast<uint8_t*>(heightMapData), resolutionX * resolutionY * sizeof(float));
}
